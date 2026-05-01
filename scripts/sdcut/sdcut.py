"""
---------------------------------------------------------------------
| Description:                                                      |
---------------------------------------------------------------------
    Script for cutting specified list of wave banks or codes from 
    audio containers for TMNT2: Battle Nexus (PC version)
    
---------------------------------------------------------------------
| Usage format:                                                     |
---------------------------------------------------------------------
    python sdcut.py <list_path> <command>
    
    <list_path> is path to list to cut from sound container.
    It maybe code list for stream wave container or bank list for se 
    wave container.
                          
---------------------------------------------------------------------
| <command> list:                                                   |
---------------------------------------------------------------------
    --secut <path>              - Performs cutting for SE wave container.
                                  <path> is an path to WaveData.bin container.
    
    --strcut <path codebase>    - Performs cutting for STREAM wave container.
                                  <path> is an path to voiceX.bin or 
                                  strbgm.vas container.
                                  <codebase> is bank base code value.
"""
import sys
import os
import os.path
import struct


def sd_print(msg: str):
    print(f"sd: {msg}")


def sd_align_int(value: int, alignment: int = 64) -> int:
    if value % alignment == 0:
        return value
    return value + (alignment - (value % alignment))


def sd_align_adjust(value: int, alignment: int = 64) -> int:
    return (sd_align_int(value, alignment) - value)


def sd_read_int_list_file(fname) -> list[int]:
    intlist: list[int] = []
    try:
        with open(fname, "r") as fptr:
            for line in fptr:
                content = line.strip()
                if content:
                    intlist.append(int(content, 0))
    except Exception as e:
        sd_print(f"Failed to read {fname} list ({e})")
        
    return intlist


def sd_align_fptr(fptr, align):
    fpos = fptr.tell()
    fpad = sd_align_adjust(fpos, align)
    if fpad > 0:
        fptr.write(b'\x00' * fpad)


class SdSeBank:
    def __init__(self):
        self.count: int = 0
        self.flag: int = 0
        self.offset: int = 0
        self.waveOffset: int = 0
        self.waveSize: int = 0
        self.isToCut: bool = False


class SdSeDrum:
    def __init__(self):
        self.count: int = 0
        self.offset: int = 0


class SdSeHeader:
    def __init__(self):
        self.title: str = ""
        self.version: int = 0
        self.totalSize: int = 0
        self.envSize: int = 0
        self.waveSize: int = 0
        self.drum: SdSeDrum = SdSeDrum()
        self.bank: dict[int, SdSeBank] = {}
        self.bankCount: int = 0


class SdDmaInfo:
    def __init__(self, size: int, block: int):
        self.size = size
        self.block = block


class SdStrFile:
    def __init__(self, offset: int, playbacktime: int):
        self.offset = offset
        self.playbacktime = playbacktime
        self.size: int = 0
        self.isToCut: bool = False


class SdStrHeader:
    def __init__(self):
        self.size: int = 0
        self.count: int = 0
        self.file: dict[int, SdStrFile] = {}
        self.page: SdDmaInfo = SdDmaInfo(0, 0)


class SdCut:
    DEF_SCT_SIZE: int = 2048


    def __init__(self):
        self.isStr: bool = False
        self.str: SdStrHeader = SdStrHeader()
        self.se: SdSeHeader = SdSeHeader()
        self.fptr = None        
        self.fpath: str = ""
        self.cutlist: list[str] = []
        self.cutmask: int
        self.elist_base_id: int 


    def __del__(self):
        self.close()


    def open_stream(self, path: str) -> bool:
        try:
            self.fptr = open(path, "rb")
        except Exception as e:
            return False
        
        self.fptr.seek(0, os.SEEK_END)
        fileSize = self.fptr.tell()
        self.fptr.seek(0, os.SEEK_SET)
        self.fpath = path
        self.isStr = True
        
        self.str.size = struct.unpack('<I', self.fptr.read(4))[0]
        self.str.count = struct.unpack('<I', self.fptr.read(4))[0]
    
        for i in range(self.str.count): 
            offset = struct.unpack('<I', self.fptr.read(4))[0]
            playbacktime = struct.unpack('<I', self.fptr.read(4))[0]
            self.str.file[i] = SdStrFile(offset, playbacktime)

        self.fptr.read(4)
        self.str.page.size = struct.unpack('<I', self.fptr.read(4))[0]
        self.str.page.block = struct.unpack('<I', self.fptr.read(4))[0]

        for i in range(self.str.count):
            offsetCurr = self.str.file[i + 0].offset
            offsetNext = self.str.file[i + 1].offset if i < self.str.count - 1 else fileSize
            streamSize = offsetNext - offsetCurr
            self.str.file[i].size = streamSize
            self.str.file[i].isToCut = True
        
        return True
    

    def open_se(self, path: str) -> bool:
        try:
            self.fptr = open(path, "rb")
        except Exception as e:
            return False
        
        self.fptr.seek(0, os.SEEK_END)
        fileSize = self.fptr.tell()
        self.fptr.seek(0, os.SEEK_SET)
        self.fpath = path
        self.isStr = True

        self.se.title = struct.unpack('<32s', self.fptr.read(32))[0]
        self.se.version = struct.unpack('<I', self.fptr.read(4))[0]
        self.se.totalSize = struct.unpack('<I', self.fptr.read(4))[0]
        self.se.envSize = struct.unpack('<I', self.fptr.read(4))[0]
        self.se.waveSize = struct.unpack('<I', self.fptr.read(4))[0]

        self.se.drum.count = struct.unpack('<H', self.fptr.read(2))[0]
        self.fptr.read(2)
        self.se.drum.offset = struct.unpack('<I', self.fptr.read(4))[0]

        for i in range(256):
            bank = SdSeBank()
            bank.count = struct.unpack('<H', self.fptr.read(2))[0]
            bank.flag = struct.unpack('<H', self.fptr.read(2))[0]
            bank.offset = struct.unpack('<I', self.fptr.read(4))[0]
            bank.waveOffset = struct.unpack('<I', self.fptr.read(4))[0]
            bank.waveSize = struct.unpack('<I', self.fptr.read(4))[0]            
            bank.isToCut = True
            
            self.se.bank[i] = bank
            if (bank.count > 0):
                self.se.bankCount = self.se.bankCount + 1

        return True


    def close(self):
        if self.fptr is not None:
            self.fptr.close()


    def is_codemask_hit(self, code) -> bool:
        return True if (code & 0xF000) == self.cutmask else False


    def get_index_from_code(self, code) -> int:
        return (code & 0x0FFF)
    

    def cut_code_list(self, path):
        codelist = sd_read_int_list_file(path)

        for code in codelist:
            if not self.is_codemask_hit(code):
                continue
            index = self.get_index_from_code(code)
            if (index >= 0 and index < self.str.count):
                self.str.file[index].isToCut = False
            else:
                sd_print(f"Invalid index of file: {index}")

        basename = os.path.basename(self.fpath)
        
        try:
            with open(basename, "wb") as fptr:
                # write header
                data = bytearray()
                data.extend(struct.pack('<I', self.str.size))
                data.extend(struct.pack('<I', self.str.count))
                
                for i in range(self.str.count):
                    if self.str.file[i].isToCut:
                        data.extend(struct.pack('<I', 0))
                        data.extend(struct.pack('<I', 0))
                    else:
                        data.extend(struct.pack('<I', self.str.file[i].offset))
                        data.extend(struct.pack('<I', self.str.file[i].playbacktime))

                data.extend(struct.pack('<I', 2))
                data.extend(struct.pack('<I', self.str.page.size))
                data.extend(struct.pack('<I', self.str.page.block))
   
                fptr.write(data)
                sd_align_fptr(fptr, self.str.size)

                # write data
                for i in range(self.str.count):
                    # skip cut files
                    if self.str.file[i].isToCut:
                        continue

                    # write wave data from org & update offset
                    offset = fptr.tell()
                    self.fptr.seek(self.str.file[i].offset, os.SEEK_SET)
                    fptr.write(self.fptr.read(self.str.file[i].size))
                    self.str.file[i].offset = offset

                    # align sct
                    sd_align_fptr(fptr, self.DEF_SCT_SIZE)

                # write new offsets
                data.clear()
                for i in range(self.str.count):
                    if self.str.file[i].isToCut:
                        data.extend(struct.pack('<I', 0))
                        data.extend(struct.pack('<I', 0))
                    else:
                        data.extend(struct.pack('<I', self.str.file[i].offset))
                        data.extend(struct.pack('<I', self.str.file[i].playbacktime))

                fptr.seek(8, os.SEEK_SET)
                fptr.write(data)

        except Exception as e:
            sd_print("Failed while cut code IO: " + str(e))

        return False
    

    def cut_bank_list(self, path):
        banklist = sd_read_int_list_file(path)

        for bank in banklist:
            if (bank >= 0 and bank < self.se.bankCount):
                self.se.bank[bank].isToCut = False
            else:
                sd_print(f"Invalid bank index: {bank} "
                         f"(max count {self.se.bankCount})")

        basename = os.path.basename(self.fpath)
        
        try:
            with open(basename, "wb") as fptr:
                # write header & drum
                self.fptr.seek(0, os.SEEK_SET)
                fptr.write(self.fptr.read(self.se.envSize))
                fptr.write(self.fptr.read(self.se.envSize))

                sd_align_fptr(fptr, self.DEF_SCT_SIZE)

                # write data
                for i in range(self.se.bankCount):
                    # skip cut files
                    if self.se.bank[i].isToCut:
                        self.se.bank[i].count = 0
                        self.se.bank[i].waveOffset = 0
                        self.se.bank[i].waveSize = 0
                        continue

                    # write wave data from org & update offset
                    offset = fptr.tell()
                    self.fptr.seek(self.se.bank[i].waveOffset, os.SEEK_SET)
                    fptr.write(self.fptr.read(self.se.bank[i].waveSize))
                    self.se.bank[i].waveOffset = offset

                    # align sct
                    sd_align_fptr(fptr, self.DEF_SCT_SIZE)

                # write new offsets
                data = bytearray()
                for i in range(self.se.bankCount):
                    data.extend(struct.pack('<H', self.se.bank[i].count))
                    data.extend(struct.pack('<H', self.se.bank[i].flag))
                    data.extend(struct.pack('<I', self.se.bank[i].offset))
                    data.extend(struct.pack('<I', self.se.bank[i].waveOffset))
                    data.extend(struct.pack('<I', self.se.bank[i].waveSize))

                fptr.seek(56, os.SEEK_SET)
                fptr.write(data)

        except Exception as e:
            sd_print("Failed while cut bank IO: " + str(e))

        return False


def main():
    if len(sys.argv) < 2:
        sd_print("Too few args for run")
        return
    
    sdcut = SdCut()
    cutlist = sys.argv[1]
    
    # Check for command
    for i, arg in enumerate(sys.argv[1:], start=1):
        if sys.argv[i] == '--secut':
            if sdcut.open_se(sys.argv[i + 1]):
                sdcut.cut_bank_list(cutlist)
                sdcut.close()
            break

        if sys.argv[i] == '--strcut':
            if sdcut.open_stream(sys.argv[i + 1]):
                sdcut.cutmask = int(sys.argv[i + 2], 0)
                sdcut.cut_code_list(cutlist)
                sdcut.close()
            break


if __name__ == "__main__":
    main()