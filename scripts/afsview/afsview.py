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

---------------------------------------------------------------------
| <command> list:                                                   |
---------------------------------------------------------------------
    --print             - prints afs content
    
    --eall              - extract ALL content from afs archive.
                          <afs_path> is path to afs archive
                          
    --efid <id ...>     - extract file from afs archive by id. 
                          <afs_path> is path to afs archive
                          <id ...> is space separated ids to read OR 
                          path to list file with ids each at new line
"""
import sys
import os
import os.path
import struct


def afs_print(msg: str):
    print(f"AFS: {msg}")


def afs_read_list_file(fname) -> list[str]:
    list = []
        
    try:
        fptr = open(fname, "r")        
        list = fptr.read().splitlines()
        fptr.close()
    except Exception as e:
        afs_print(f"Failed to read {fname} ({e})")
        
    return list


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
    def __init__(self):
        self.info: AfsInfo = AfsInfo()
        self.fptr = None
        self.out_path: str = ""
        self.hex_print: bool = False


    def __del__(self):
        self.close()


    def open(self, path: str) -> bool:    
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

        try:
            fout = open(self.correct_out_path(fname), "wb")
            fout.write(self.fptr.read(fsize))
            fout.close()
        except Exception as e:
            afs_print(f"Failed to write file {fname} ({e})")


    def extract_fid_list(self, fid_list: list[int]):
        for fid in fid_list:
            self.extract_fid(fid)


    def extract_all(self):
        for fid in range(self.info.fcount):
            self.extract_fid(fid)


def get_args_list(idx, as_int: bool = True) -> list:
    list = []
    
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

    # Check for params
    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--hex':
            afsview.hex_print = True

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
            afsview.extract_all()
            break
        
        if sys.argv[i] == '--efid':
            fid_list = get_args_list(i + 1)
            afsview.extract_fid_list(fid_list)
            break

    afsview.close()


if __name__ == "__main__":
    main()