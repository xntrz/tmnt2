"""
---------------------------------------------------------------------
| Description:                                                      |
---------------------------------------------------------------------
    Script for extracting AFS archive data
    
---------------------------------------------------------------------
| Usage format:                                                     |
---------------------------------------------------------------------
    python afsview.py <afs_path> [params] <command> <args>

---------------------------------------------------------------------
| [params] list (Optional):                                         |
---------------------------------------------------------------------
    --hex               - sets hex format for offset and size for print

    --mexts <exts...>   - sets extension file filter for make archive.
                          <exts...> is one or more file extensions 
                          separated by space to allow to write in archive.

    --mpcut <paths...>  - sets paths to cut from final filename table.

    --elist <path...>   - sets path to input lsit for extract.
                          if no list specified for extract all names
                          will be taken from nametable

---------------------------------------------------------------------
| <command> list:                                                   |
---------------------------------------------------------------------
    --print             - prints afs content
    
    --eall <path ...>   - extract ALL content from afs archives.
                          <afs_path> is ignored here.
                          <path ...> paths to afs containers to extract
                          if elist specified it should be equal to total
                          file count of all provided archives otherwise
                          failure
                          
    --efid <id ...>     - extract file from afs archive by id. 
                          <afs_path> is path to afs archive
                          <id ...> is space separated ids to read OR 
                          path to list file with ids each at new line
    
    --mafs <path ...>   - makes afs archive from specified dir path
                          <afs_path> is ignored here
                          <path...> is path separated by spaces of dir
                          to write to archive. 
                          If dir contains file named like dir itself and 
                          with extension .list - it will be used to get 
                          what need to write, if file not exist whole dir 
                          will be written.
"""
import sys
import os
import os.path
import struct


def afs_print(msg: str):
    print(f"AFS: {msg}")


def afs_align_int(value: int, alignment: int = 64) -> int:
    if value % alignment == 0:
        return value
    return value + (alignment - (value % alignment))


def afs_align_adjust(value: int, alignment: int = 64) -> int:
    return (afs_align_int(value, alignment) - value)


def afs_read_list_file(fname) -> list[str]:
    list = []
        
    try:
        fptr = open(fname, "r")        
        list = fptr.read().splitlines()
        fptr.close()
    except Exception as e:
        afs_print(f"Failed to read {fname} ({e})")
        
    return list


def afs_get_files_recursive(dir):
    files = []
    for root, dirnames, filenames in os.walk(dir):
        for filename in filenames:
            files.append(os.path.join(root, filename))
    return files


class AfsMakeFileInfo:
    def __init__(self, size: int, offset: int):
        self.size = size
        self.offset = offset


class AfsMakeArchive:
    def __init__(self, name: str, list_path: str, file_list: list[str]):
        self.name = name
        self.list_path = list_path
        self.file_list = file_list
        self.file_info: list[AfsMakeFileInfo] = []


class AfsMakeInfo:
    def __init__(self):
        self.archives: list[AfsMakeArchive] = []
        self.nametable: AfsMakeFileInfo = AfsMakeFileInfo(0, 0)
        self.fcount: int = 0


class AfsFile:
    def __init__(self, 
                 offset: int, 
                 size: int, 
                 name: str):
        self.offset = offset
        self.size = size
        self.name = name


class AfsInfo:
    def __init__(self):
        self.fcount: int = 0
        self.ftable: dict[int, AfsFile] = {}


class AfsView:
    DEF_SCT_SIZE: int = 2048


    def __init__(self):
        self.info: AfsInfo = AfsInfo()
        self.fptr = None
        self.out_path: str = ""
        self.hex_print: bool = False
        self.make_call: bool = False
        self.make_ext_filter: list[str] = []
        self.make_cut_path: list[str] = []
        self.header_out_fname: str = "FnameTable.hpp"
        self.elist: list[str] = []
        self.elist_base_id: int 


    def __del__(self):
        self.close()


    def open(self, path: str) -> bool:    
        if (self.make_call):
            return True
        
        try:
            self.fptr = open(path, "rb")
        except Exception as e:
            return False
        
        magic = self.fptr.read(4)
        if (magic != b'AFS\x00'):
            afs_print("Not a AFS archive!")
            return False
        
        self.info.fcount = struct.unpack('<I', self.fptr.read(4))[0]        

        # Read file data table (+1 for offset to an name table)
        for i in range(self.info.fcount + 1): 
            foffset = struct.unpack('<I', self.fptr.read(4))[0]
            fsize = struct.unpack('<I', self.fptr.read(4))[0]
            fname = "" if i < (self.info.fcount - 1) else "__NAMETABLE__"
            self.info.ftable[i] = AfsFile(foffset, fsize, fname)

        # Read file name info
        self.fptr.seek(self.info.ftable[self.info.fcount].offset, os.SEEK_SET)

        # Read file name table
        for i in range(self.info.fcount):
            finfo = self.fptr.read(48)
            fname = finfo[0:32]
            fname = fname.split(b'\x00')[0].decode('ascii', errors='ignore')
            self.info.ftable[i].name = fname

        return True
    

    def close(self):
        if self.fptr is not None:
            self.fptr.close()


    def is_in_path_cut(self, file_path: str) -> str:
        file_path = file_path.replace('\\', '/')

        for parent in self.make_cut_path:
            parent = parent.replace('\\', '/')

            if not parent.endswith('/'):
                parent += '/'

            if file_path.startswith(parent):
                return parent

        return None


    def make_h_file(self, minfo: AfsMakeInfo):
        tab = "    "
        hline = []
        hline.append("")
        hline.append("#if !defined(FILETABLE_IMPL)")
        hline.append("")
        hline.append("namespace FILEID")
        hline.append("{")
        hline.append(f"{tab}enum")
        hline.append(f"{tab}{{")

        total_files = 0
        id_offset = 0
        for archive in minfo.archives:
            file_count = len(archive.file_list)
            total_files = total_files + file_count
            hline.append(f"{tab}{tab}{archive.name}_BEGIN = {id_offset},")
            hline.append(f"{tab}{tab}{archive.name}_END   = {id_offset + file_count},")
            hline.append(f"{tab}{tab}{archive.name}_MAX   = ({archive.name}_END - {archive.name}_BEGIN),")
            hline.append("")
            id_offset = id_offset + file_count

        total_files_summ = "("
        archive_no = 0
        archive_num = len(minfo.archives)
        for archive in minfo.archives:
            total_files_summ += (archive.name + "_MAX")
            archive_no = archive_no + 1
            if (archive_no < archive_num):
                total_files_summ += " + "
        total_files_summ += ")"

        hline.append(f"{tab}{tab}FILEID_MAX = {total_files_summ},")
        hline.append(f"{tab}}};")
        hline.append("};")
        hline.append("")
        hline.append("#else /* !defined(FILETABLE_IMPL) */")
        hline.append("")
        hline.append("/*static*/ const char* CFilename::m_apszFilename[] =")
        hline.append("{")

        file_list_cut = []
        for archive in minfo.archives:
            for file_path in archive.file_list:
                path_to_cut = self.is_in_path_cut(file_path)
                file_path = file_path.replace('\\', '/')
                if path_to_cut is not None:
                    file_path = file_path.replace(path_to_cut, "")
                file_list_cut.append(file_path)
            
        for file_path in file_list_cut:
            hline.append(f"{tab}\"{file_path}\",")

        hline.append("};")
        hline.append("")
        hline.append("#endif /* !defined(FILETABLE_IMPL) */")
        
        try:
            fout = open(self.header_out_fname, "w")
            for line in hline:
                fout.write(f"{line}\n")
            fout.close()
        except Exception as e:
            afs_print(f"Failed to write file {self.header_out_fname} while making .h header ({e})")


    def make_reserve_header(self, fptr, minfo: AfsMakeInfo, marchive: AfsMakeArchive):
        data = bytearray()

        # write main header
        data.extend(struct.pack('<4s', b'AFS\x00'))
        data.extend(struct.pack('<I', 0))
        
        # write empty data for now just for space reserve
        for file in marchive.file_list:
            minfo.fcount = minfo.fcount + 1
            data.extend(struct.pack('<I', 0))
            data.extend(struct.pack('<I', 0))

        # write nametable info
        data.extend(struct.pack('<I', 0))
        data.extend(struct.pack('<I', 0))

        data.extend(bytes(afs_align_adjust(len(data), self.DEF_SCT_SIZE)))
        fptr.write(data)


    def make_write_header(self, fptr, minfo: AfsMakeInfo, marchive: AfsMakeArchive):
        data = bytearray()

        # write file count & file info
        data.extend(struct.pack('<I', minfo.fcount))
        for file in marchive.file_info:
            data.extend(struct.pack('<I', file.offset))
            data.extend(struct.pack('<I', file.size))

        # write nametable info
        data.extend(struct.pack('<I', minfo.nametable.offset))
        data.extend(struct.pack('<I', minfo.nametable.size))

        fptr.seek(4, os.SEEK_SET)
        fptr.write(data)
    

    def make_write_archive(self, fptr, marchive: AfsMakeArchive):
        for file in marchive.file_list:
            try:
                fsize = os.path.getsize(file)
                marchive.file_info.append(AfsMakeFileInfo(fsize, fptr.tell()))

                fin = open(file, "rb")
                if (fsize == 0):
                    fptr.write(bytes(self.DEF_SCT_SIZE))
                else:
                    fptr.write(fin.read())
                fptr.write(bytes(afs_align_adjust(fsize, self.DEF_SCT_SIZE)))
                fin.close()
            except Exception as e:
                afs_print(f"Failed to write file {file} while making archive ({e})")


    def make_write_trailer(self, fptr, minfo: AfsMakeInfo, marchive: AfsMakeArchive):
        data = bytearray()
        
        minfo.nametable.offset = fptr.tell()
        minfo.nametable.size = 48 * minfo.fcount

        # write nametable
        for file in marchive.file_list:
            basename = os.path.basename(file)
            data.extend(struct.pack('<32s', basename.encode('ascii')))
            data.extend(struct.pack('<I', 0)) # skip metadata
            data.extend(struct.pack('<I', 0)) # skip metadata
            data.extend(struct.pack('<I', 0)) # skip metadata
            data.extend(struct.pack('<I', 0)) # skip metadata                
        
        data.extend(bytes(afs_align_adjust(len(data), self.DEF_SCT_SIZE)))
        fptr.write(data)


    def make(self, lists_path: list[str]):
        minfo = AfsMakeInfo()
        
        for list_path in lists_path:
            basename = os.path.basename(list_path)
            fext = os.path.splitext(basename)[1]
            if fext != ".list":
                afs_print(f"{fext} not a .list file!")
                continue
            
            file_list_raw = afs_read_list_file(list_path)
            file_list = []

            if self.make_ext_filter:
                for file_path in file_list_raw:
                    _basename = os.path.basename(file_path)
                    _fext = os.path.splitext(_basename)[1]
                    if _fext in self.make_ext_filter:
                        file_list.append(file_path)
            else:
                file_list = file_list_raw                

            if file_list:
                fname = os.path.splitext(basename)[0]
                minfo.archives.append(AfsMakeArchive(fname.upper(), list_path, file_list))
        
        for i, list_path in enumerate(lists_path):
            minfo.fcount = 0
            list_path_basename = os.path.basename(list_path)
            list_path_name = os.path.splitext(list_path_basename)[0]
            list_path_name = list_path_name + ".dat"
            try:
                fout = open(list_path_name, "wb")
                self.make_reserve_header(fout, minfo, minfo.archives[i])
                self.make_write_archive(fout, minfo.archives[i])
                self.make_write_trailer(fout, minfo, minfo.archives[i])
                self.make_write_header(fout, minfo, minfo.archives[i])
                fout.close()
            except Exception as e:
                afs_print(f"Failed to write file {list_path_name} while making afs ({e})")

        self.make_h_file(minfo)


    def print(self):
        afs_print('-' * 73)
        afs_print(f"| {'ID':<4} | {'Name':<32} | {'Size':<12} | {'Offset':<12} |")
        afs_print('-' * 73)
        for i, finfo in self.info.ftable.items():
            if self.hex_print:
                afs_print(f"| {i:<4} | {finfo.name:<32} | {finfo.size:<12X} | {finfo.offset:<12X} |")
            else:                
                afs_print(f"| {i:<4} | {finfo.name:<32} | {finfo.size:<12} | {finfo.offset:<12} |")
        afs_print('-' * 73)


    def correct_out_path(self, fpath) -> str:
        dir = "extract"
        
        if not (os.path.exists(dir)):
                os.makedirs(dir)
                                                
        return os.path.join(dir, fpath)
    

    def extract_fid(self, fid: int):
        foffset = self.info.ftable[fid].offset
        fsize = self.info.ftable[fid].size
        fname = self.info.ftable[fid].name

        self.fptr.seek(foffset, os.SEEK_SET)

        if self.elist:
            fpath = self.elist[self.elist_base_id + fid]
            fpath = self.correct_out_path(fpath)
            fpath = os.path.normpath(fpath)
            fdirname = os.path.dirname(fpath)
            if fdirname and not os.path.exists(fdirname):
                os.makedirs(fdirname)
            fname = fpath
        else:
            fname = self.correct_out_path(fname)
        try:
            fout = open(fname, "wb")
            fout.write(self.fptr.read(fsize))
            fout.close()
        except Exception as e:
            afs_print(f"Failed to write file {fname} while extracting afs ({e})")


    def extract_fid_list(self, fid_list: list[int]):
        for fid in fid_list:
            self.extract_fid(fid)


    def extract_all(self):
        for fid in range(self.info.fcount):
            self.extract_fid(fid)


def get_args_list(idx, as_int: bool = True, ignore_read: bool = False) -> list:
    list = []
    
    if not ignore_read:
        if os.path.isfile(sys.argv[idx]):
            list_file = afs_read_list_file(sys.argv[idx])
            for fid in list_file:
                list.append( int(fid) if as_int else str(fid) )

        if list:
            return list

    idx_max = len(sys.argv)
    while (idx < idx_max) and (not sys.argv[idx].startswith("--")):
        list.append( int(sys.argv[idx]) if as_int else str(sys.argv[idx]) )
        idx = idx + 1

    return list



def main():
    if len(sys.argv) < 3:
        afs_print("Too few args for run")
        return
    
    afsview = AfsView()
    afspath = sys.argv[1]

    elist: list[str] = []

    # Check for params
    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--hex':
            afsview.hex_print = True

        if sys.argv[i] == '--eall':
            afsview.make_call = True

        if sys.argv[i] == '--mext':
            afsview.make_ext_filter = get_args_list(i + 1, False)

        if sys.argv[i] == '--mafs':
            afsview.make_call = True

        if sys.argv[i] == '--mpcut':
            afsview.make_cut_path = get_args_list(i + 1, False)

        if sys.argv[i] == '--elist':
            elist = get_args_list(i + 1, False)

    # Open afs
    if not afsview.open(afspath):
        afs_print(f"Failed to open '{afspath}'")
        return

    # Check for command
    fid_list = []

    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--print':
            afsview.print()
            break

        if sys.argv[i] == '--eall':
            idoffset = 0
            list_afs_paths = get_args_list(i + 1, False, True)
            for _afs_path in list_afs_paths:
                _afsview = AfsView()
                if not _afsview.open(_afs_path):
                    afs_print(f"Failed to open {_afs_path}")
                else:
                    _afsview.elist = elist.copy()
                    _afsview.elist_base_id = idoffset
                    _afsview.extract_all()
                    idoffset = idoffset + _afsview.info.fcount
            break
        
        if sys.argv[i] == '--efid':
            fid_list = get_args_list(i + 1)
            afsview.extract_fid_list(fid_list)
            break

        if sys.argv[i] == '--mafs':
            dir_list = get_args_list(i + 1, False, True)
            afsview.make(dir_list)
            break

    afsview.close()


if __name__ == "__main__":
    main()