"""
---------------------------------------------------------------------
| Description:                                                      |
---------------------------------------------------------------------
    Script for extracting or making .lpac for TMNT2: Battle Nexus

    TODO readers .list param description
    
---------------------------------------------------------------------
| Usage format:                                                     |
---------------------------------------------------------------------
    python lpacview.py <lpac_path> [params] <command> <args>

---------------------------------------------------------------------
| [params] list (Optional):                                         |
---------------------------------------------------------------------
    --hex               - sets hex format for offset and size for print

    --shdr              - saves game reader (main and extra) header when extract file
                          if not set file will be extracted as pure binary

---------------------------------------------------------------------
| <command> list:                                                   |
---------------------------------------------------------------------
    --print             - prints lpac content
    
    --mlpac             - makes .lpac from .list file. 
                          <lpac_path> is path to .list file
    
    --eall              - extracts ALL content from .lpac.
                          <lpac_path> is path to .lpac
                          
    --efid <id ...>     - extract file from .lpac by id. 
                          <lpac_path> is path to .lpac
                          <id ...> is space separated ids to read 
                          or path to list file with ids each at new line  
"""

import sys
import os
import os.path
import struct
from enum import IntEnum


def line():
    return sys._getframe(1).f_lineno


def lpac_print(msg: str, line: int = -1):
    if (line != -1):
        print(f"LPAC {line}: {msg}")
    else:
        print(f"LPAC: {msg}")


def lpac_str_isfloat(value: str) -> bool:
    value = value.strip("f")
    try:
        float(value)
        return True
    except ValueError:
        return False
    

def lpac_align_int(value: int, alignment: int = 64) -> int:
    if value % alignment == 0:
        return value
    return value + (alignment - (value % alignment))


def lpac_align_adjust(value: int, alignment: int = 64) -> int:
    return (lpac_align_int(value) - value)


def lpac_read_list_file(fname) -> list[str]:
    list = []
        
    try:
        fptr = open(fname, "r")        
        list = fptr.read().splitlines()
        fptr.close()
    except Exception as e:
        lpac_print(f"Failed to read {fname} ({e})")
        
    return list


class LpacChunkType(IntEnum):
    #
    # READERTYPE::VALUE
    # src/Game/System/DataLoader/DataLoaderFromBinary.cpp
    #
    INVALID                 = -1,
    MODEL                   =  0,
    MODEL_TOON              =  1,
    MOTION_SET              =  2,
    MOTION                  =  3,
    MOTION_PARAMETER        =  4,
    TEXTURE_DICTIONARY      =  7,
    MODEL_BSP               =  8,
    CAMERA                  =  9,
    EFFECT                  = 10,
    ANIM2D                  = 11,
    UVANIM                  = 12,
    FONT                    = 13,
    MAP_MODEL               = 14,
    MAP_INFO                = 15,
    ADD_TEXTURE_DICTIONARY  = 16,
    GIMMICK                 = 17,
    STRINGS                 = 18,
    ENEMY_PARAMETER         = 19,
    ICONS_PS2               = 20,
    ICONS_NGC               = 21,
    MOVIETEXT               = 22,
    CAS_C2B                 = 23,   # TMNT3


class LpacChunkExtension:
    name: dict[int, str] = {
        # mostly not a real extensions but we have somehow to match file to reader type
        LpacChunkType.MODEL:                    ".dff",
        LpacChunkType.MODEL_TOON:               ".tdff",
        LpacChunkType.MOTION_SET:               ".anm_set",
        LpacChunkType.MOTION:                   ".anm",
        LpacChunkType.MOTION_PARAMETER:         ".chr",
        LpacChunkType.TEXTURE_DICTIONARY:       ".txd",
        LpacChunkType.MODEL_BSP:                ".bsp",
        LpacChunkType.CAMERA:                   ".cmd",
        LpacChunkType.EFFECT:                   ".ef",
        LpacChunkType.ANIM2D:                   ".2d",
        LpacChunkType.UVANIM:                   ".uv",
        LpacChunkType.FONT:                     ".fnt",
        LpacChunkType.MAP_MODEL:                ".mdff",
        LpacChunkType.MAP_INFO:                 ".mpd",
        LpacChunkType.ADD_TEXTURE_DICTIONARY:   ".txd_add",
        LpacChunkType.GIMMICK:                  ".mgd",
        LpacChunkType.STRINGS:                  ".bin",
        LpacChunkType.ENEMY_PARAMETER:          ".epb",
        LpacChunkType.MOVIETEXT:                ".mtd",
        LpacChunkType.CAS_C2B:                  ".c2b", # TMNT3
    }
    value: dict[str, int] = {v: k for k, v in name.items()}


class LpacChunkHeader:
    ALIGN: int
    MAIN_SIZE: int
    EXTRA_SIZE: int


    @staticmethod
    def initialize(is_v3: bool):
        LpacChunkHeader.ALIGN = 64
        LpacChunkHeader.MAIN_SIZE = 32 if not is_v3 else 48
        LpacChunkHeader.EXTRA_SIZE = 32 if not is_v3 else 80


class LpacChunkInfo:
    def __init__(self, name, type, offset, size, skip, ehdr_offset):
        self.name: str = name
        self.type: LpacChunkType = type
        self.offset: int = offset
        self.size: int = size
        self.skip: int = skip
        self.ehdr_offset: int = ehdr_offset


class LpacInfo:
    def __init__(self):
        self.fcount: int = 0
        self.ftable: dict[int, LpacChunkInfo] = {}


class LpacMakePath:
    def __init__(self, path, params):
        self.path: str = path
        self.params: list[str] = params


class LpacMakeInfo:
    def __init__(self):
        self.fcount: int = 0
        self.ftable: dict[int, LpacMakePath] = {}


class LpacView:
    def __init__(self):
        self.info: LpacInfo = LpacInfo()
        self.fname: str = ""
        self.save_hdr: bool = False
        self.make_call: bool = False    
        self.extract_dir: str = ""
        self.last_chr: str = ""
        self.out_list: list[str] = []
        self.fptr = None
        self.hex_print: bool = False


    def __del__(self):
        self.close()


    def open(self, path: int) -> bool:
        if self.make_call:
            return True

        # open & read LPAC header
        try:
            self.fptr = open(path, "rb")
        except Exception as e:
            lpac_print(f"Failed to open '{path}'")
            return False
        
        self.fname = os.path.basename(path)
        self.fname = self.fname.replace(".lpac", "")

        magic = self.fptr.read(4)
        if (magic != b'LPAC'):
            lpac_print("Not a LPAC!")
            return False
                
        self.info.fcount = struct.unpack('<I', self.fptr.read(4))[0]

        # Align file pos to an first chunk
        fpos = self.fptr.tell()
        fpos = lpac_align_int(fpos)
        self.fptr.seek(fpos)

        fsize = os.path.getsize(path)

        for i in range(self.info.fcount):            
            fpos = self.fptr.tell()
            if (fpos >= fsize):
                lpac_print("Corrupted LPAC (file count mismatch)")
                return False

            chdrm = self.fptr.read(LpacChunkHeader.MAIN_SIZE)
            chdre_offset = self.fptr.tell()
            chdre = self.fptr.read(LpacChunkHeader.EXTRA_SIZE)
            ctype = struct.unpack('<I', chdrm[0:4])[0]
            csize = struct.unpack('<I', chdrm[4:8])[0]
            cname = chdrm[16:32].split(b'\x00')[0].decode('ascii', errors='ignore')
            
            fpos = self.fptr.tell()
            coffset = lpac_align_int(fpos, LpacChunkHeader.ALIGN)
            cskip = coffset - fpos
            
            self.info.ftable[i] = LpacChunkInfo(cname, LpacChunkType(ctype), coffset, csize, cskip, chdre_offset)
            self.fptr.seek(lpac_align_int(csize, LpacChunkHeader.ALIGN), os.SEEK_CUR)

        return True
    

    def close(self):
        if self.fptr is not None:
            self.fptr.close()


    def print(self):
        lpac_print('-' * 90)
        lpac_print(f"| {'ID':<3} | {'Name':<16} | {'Type':<31} | {'Size':<12} | {'Offset':<12} |")
        lpac_print('-' * 90)
        for i, chunk in self.info.ftable.items():
            ftype = chunk.type
            fsize = chunk.size
            fofs = chunk.offset
            fname = chunk.name
            if self.hex_print:
                 lpac_print(f"| {i:<3} | "
                            f"{fname:<16} | "
                            f"{ftype.value:<} {ftype.name:<28} | "
                            f"{fsize:<12X} | "
                            f"{fofs:<12X} |")
            else:
                lpac_print(f"| {i:<3} | "
                           f"{fname:<16} | "
                           f"{ftype.value:<2} {ftype.name:<28} | "
                           f"{fsize:<12} | "
                           f"{fofs:<12} |")
        lpac_print('-' * 90)
    

    def correct_out_path(self, fpath: str) -> str:
        if (self.make_call):
            return fpath
        
        dir = "extract"

        if not (os.path.exists(dir)):
                os.makedirs(dir)
                                                
        return os.path.join(dir, fpath)
    

    def save_extract_list(self, fpath: str):
        try:                
            fptr = open(fpath, "w")
            for path in self.out_list:
                fptr.write(f"{os.path.relpath(path)}\n")
            fptr.close()
        except Exception as e:
            lpac_print(f"Failed to write file {fpath} ({e})")


    def write_extract_list(self, fpath: str, fid: int):
        fparam = ""
        ehdr_offset = self.info.ftable[fid].ehdr_offset
        ftype = self.info.ftable[fid].type
        fname =  self.info.ftable[fid].name

        self.fptr.seek(ehdr_offset, os.SEEK_SET)
        chdre = self.fptr.read(LpacChunkHeader.EXTRA_SIZE)

        param_start = ";"
            
        if (ftype == LpacChunkType.MODEL):
            name = chdre[0:16].split(b'\x00')[0].decode('ascii', errors='ignore')
            fparam = fparam + param_start + " " + name

        if (ftype == LpacChunkType.MODEL_TOON):
            name = chdre[0:16].split(b'\x00')[0].decode('ascii', errors='ignore')
            pat = struct.unpack('<I', chdre[16:20])[0]
            fparam = fparam + param_start + " " + name + ", " + str(pat)

        if (ftype == LpacChunkType.MOTION_SET):
            flag = struct.unpack('<I', chdre[0:4])[0]
            name = chdre[4:20].split(b'\x00')[0].decode('ascii', errors='ignore')
            fparam = fparam + param_start + " " + str(flag) + ((", " + name) if flag else "")

        if (ftype == LpacChunkType.ADD_TEXTURE_DICTIONARY):
            fparam = fparam + param_start + " " + fname

        if (ftype == LpacChunkType.MAP_MODEL):
            name = chdre[0:16].split(b'\x00')[0].decode('ascii', errors='ignore')
            mode = struct.unpack('<I', chdre[16:20])[0]
            fparam = fparam + param_start + " " + name + ", " + str(mode)
        
        self.out_list.append(fpath + fparam)


    def extract_fid(self, fid: int):
        foffset = self.info.ftable[fid].offset
        fsize = self.info.ftable[fid].size
        ftype = self.info.ftable[fid].type
        fskip = self.info.ftable[fid].skip
        fname = self.info.ftable[fid].name
        
        if self.save_hdr:
            hdr_size = fskip + LpacChunkHeader.MAIN_SIZE + LpacChunkHeader.EXTRA_SIZE
            fsize = fsize + hdr_size
            self.fptr.seek(foffset - hdr_size, os.SEEK_SET)
        else:
            self.fptr.seek(foffset, os.SEEK_SET)

        try:
            fdir = self.extract_dir
            if ftype == LpacChunkType.MOTION_SET:
                self.last_chr = fname
                fdir_prev = fdir
                fdir = os.path.join(fdir, fname)
                os.makedirs(fdir, exist_ok = True)
                fdir = fdir_prev
            
            if ftype == LpacChunkType.MOTION:
                fdir = os.path.join(fdir, self.last_chr)
            
            fname = fname + LpacChunkExtension.name[ftype.value]
            lpac_print(f"Extracting {fname}...")

            fpath = os.path.join(fdir, fname)
            fpath = os.path.normpath(fpath)
            
            fout = open(fpath, "wb")
            fout.write(self.fptr.read(fsize))
            fout.close()
            
            replace_str = self.correct_out_path("")
            self.write_extract_list(fpath.replace(replace_str, ""), fid)
        except Exception as e:
            lpac_print(f"Failed to write file {fname} ({e})", line())


    def extract_all(self):
        fid_list = []
        for i in range(self.info.fcount):
            fid_list.append(i)

        self.extract_fid_list(fid_list)        


    def extract_fid_list(self, fid_list: list[int]):
        self.extract_dir = self.fname
        self.extract_dir = self.correct_out_path(self.extract_dir)
        os.makedirs(self.extract_dir, exist_ok = True)

        for i, fid in enumerate(fid_list):
            self.extract_fid(fid)

        fpath = os.path.basename(self.extract_dir) + ".list"
        fpath = self.correct_out_path(fpath)
        self.save_extract_list(fpath)


    def make(self, fpath: str):
        # now handle main make call
        path_list = lpac_read_list_file(fpath)
        if not path_list:
            lpac_print("path list is empty!")
            return
        
        # Prepare make info
        mkinfo = LpacMakeInfo()

        for i, path in enumerate(path_list):            
            params_org = []
            
            path = os.path.join(os.path.dirname(fpath), path)

            # parse path string for params according to format "; <value>, ..."
            param_pos = path.find(";")            
            if (param_pos != -1):
                params_str = path[param_pos + 1:].replace(" ", "")
                params_org = params_str.split(',')
                path = path[:param_pos]                

            # correct params - currently only float
            params = []
            for j, param in enumerate(params_org):
                params.append(param.strip("f") if lpac_str_isfloat(param) else param)

            mkinfo.ftable[i] = LpacMakePath(path, params)

        # Write LPAC header
        data = bytearray()
        data.extend(struct.pack('<4s', b'LPAC')) # uMagic        
        data.extend(struct.pack('<I', 0))        # at the end we will change it to actual fcount
        data.extend(bytes(lpac_align_adjust(len(data), LpacChunkHeader.ALIGN)))
        
        # Write files
        for i, item in mkinfo.ftable.items():
            # get file base info            
            if not (os.path.exists(item.path)):
                lpac_print(f"File not found '{item.path}'")
                continue

            fname_base = os.path.basename(item.path)
            fname = os.path.splitext(fname_base)[0]
            fext = os.path.splitext(fname_base)[1]
            fsize = os.path.getsize(item.path)
            is_motion_set = False

            if os.path.isdir(item.path):
                fext = LpacChunkExtension.name[LpacChunkType.MOTION_SET]
                fsize = 0
                is_motion_set = True

            if len(fname) >= 16:
                lpac_print(f"File name too long '{item.path}")
                continue

            if not fext in LpacChunkExtension.value:
                lpac_print(f"File '{item.path}' extension unknown '{fext}'")
                continue

            # write main header        
            data.extend(struct.pack('<I', LpacChunkExtension.value[fext]))  # uType
            data.extend(struct.pack('<I', fsize))                           # uSize
            data.extend(struct.pack('<I', 0))                               # uReserved[0]
            data.extend(struct.pack('<I', 0))                               # uReserved[1]
            szNamePos = len(data)
            data.extend(struct.pack('<16s', fname.encode('ascii')))         # szName[16]

            # write extra header if exist (up to 32 bytes max)
            if item.params:                
                hdr_size = 0
                hdr_size_max = LpacChunkHeader.EXTRA_SIZE
                max_param_size = 16 # char[16] TODO probably is larger than 16 bytes for TMNT3
                min_param_size = 4  # int
                
                for param in item.params:
                    if (hdr_size + min_param_size) >= max_param_size:
                        break

                    if param.isdigit():
                        data.extend(struct.pack('<I', int(param)))
                        hdr_size = hdr_size + 4
                    #elif (lpac_str_isfloat(param)):
                    #    data.extend(struct.pack('<f', float(param)))
                    #    extra_hdr_size = extra_hdr_size + 4
                    else:
                        # for ".txd_add" make chunk name from param - where to add .txd
                        if (LpacChunkExtension.value[fext] == LpacChunkType.ADD_TEXTURE_DICTIONARY):
                            data[szNamePos:szNamePos+16] = struct.pack('<16s', param.encode('ascii'))
                        else:                                                                       
                            data.extend(struct.pack('<16s', param.encode('ascii')))
                        hdr_size = hdr_size + 16

                    if hdr_size >= hdr_size_max:
                        break

                data.extend(bytes(lpac_align_adjust(len(data), 32)))
            else:
                data.extend(bytes(32))

            # write file content
            if not is_motion_set:
                lpac_print(f"Writing {item.path}...")

                try:                
                    fptr = open(item.path, "rb")
                    data.extend(fptr.read(fsize))
                    fptr.close()
                    mkinfo.fcount = mkinfo.fcount + 1
                except Exception as e:
                    lpac_print(f"Failed to read file {item.path} ({e})")
            else:
                mkinfo.fcount = mkinfo.fcount + 1

            # align trailer
            data.extend(bytes(lpac_align_adjust(len(data), 64)))
        
        data[4:8] = struct.pack('<I', mkinfo.fcount)   # uChunkNum

        path = fpath.replace(".list", ".lpac")
        path = os.path.normpath(path)

        lpac_print(f"Saving to {path}")

        try:
            fptr = open(path, "wb")
            fptr.write(data)
            fptr.close()
        except Exception as e:
            lpac_print(f"Failed to write final file {fname} ({e})")


def get_args_list(idx, as_int: bool = True) -> list:
    list = []
    
    if os.path.isfile(sys.argv[idx]):
        list_file = lpac_read_list_file(sys.argv[idx])
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
        lpac_print("Too few args for run")
        return
    
    lpacview = LpacView()
    lpacpath = sys.argv[1]
    is_v3 = False

    # Check for params
    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--hex':
            lpacview.hex_print = True

        if sys.argv[i] == '--shdr':
            lpacview.save_hdr = True

        if sys.argv[i] == '--mlpac':
            lpacview.make_call = True
    
    LpacChunkHeader.initialize(is_v3)

    # open lpac
    if not lpacview.open(lpacpath):
        return

    # Check for command
    fid_list = []

    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--print':
            lpacview.print()
            break

        if sys.argv[i] == '--mlpac':
            lpacview.make(lpacpath)
            break

        if sys.argv[i] == '--eall':
            lpacview.extract_all()
            break
        
        if sys.argv[i] == '--efid':
            fid_list = get_args_list(i + 1)
            lpacview.extract_fid_list(fid_list)
            break

    lpacview.close()


if __name__ == "__main__":
    main()