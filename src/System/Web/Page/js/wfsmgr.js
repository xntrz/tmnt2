/////////////////////////////////////////////////////////////////////////////////
//
//  WorkerFS handling.
//
//  Callbacks prototypes & description for C code:
//
//      void onWorkerFSReady(void) 
//          Fires when worker fs is initialized and ready.
//
//      void onWorkerFSMountReady(void)
//          Fires when worker fs is mounted and ready.
//
//      void onWorkerFSMountCancel(void)
//          Fires when worker fs mounting was canceled.
//
//      void onWorkerFSDataReady(const char* data, int size, void* param) 
//          Fires when read request is complete - to know whether it success 
//          or fail check 'data' or 'size' arg. If its fail they are both zero.
//          After callback return 'data' will be invalidated.
//
//////////////////////////////////////////////////////////////////////////////////

const WORKERFS_MODE = {
    JS: 0,
    CCODE: 1,
}

let workerfs = null;
let workerfsRef = 0;
let workerfsMode = WORKERFS_MODE.CCODE;
let workerfsMountCache = {}; /* Storage to keep files while workerfs shutdown to
                                avoid second file selection after reset.
                                Format is [path, files] */

const workerFSCache = {
    smallPtr: 0,
    smallSize: 0,
    THRESHOLD: ((32768 * 4) * 2)
};

function allocWorkerFSDataBuff(size) {
    if (size <= workerFSCache.THRESHOLD) {
        if ((workerFSCache.smallPtr === 0) ||
            (size > workerFSCache.smallSize)) {

            if (workerFSCache.smallPtr !== 0) {
                Module._free(workerFSCache.smallPtr);
                workerFSCache.smallPtr = 0;
            }

            workerFSCache.smallSize = size;
            workerFSCache.smallPtr = Module._malloc(size);
        }

        return workerFSCache.smallPtr;
    }

    return Module._malloc(size);
}

function freeWorkerFSDataBuff(buff) {
    if ((buff == null) ||
        (buff === 0)) {
        return;
    }

    if (buff === workerFSCache.smallPtr) {
        return;
    }

    Module._free(buff);
}

function initializeDummyWorkerFS() {
    initializeWorkerFS(true);
}

function initializeWorkerFS(isDummy = false) {
    if (workerfsRef++)
        return;

    workerfs = new Worker("./js/wfs.js");
    workerfs.onmessage = function (msg) {
        const cmd = msg.data.cmd;

        if ((cmd == "INIT_READY") ||
            (cmd == "INIT_DUMMY_READY")) {
            if (workerfsMode == WORKERFS_MODE.JS) {
                if (typeof onWorkerFSReady == "function") {
                    onWorkerFSReady();
                }
            } else if (workerfsMode == WORKERFS_MODE.CCODE) {
                if (Module._onWorkerFSReady) {
                    Module._onWorkerFSReady();
                }
            }
        } else if (cmd == "MOUNT_READY") {
            const blobs = msg.data.blobs;
            const path = msg.data.path;

            cacheWorkerFSAddFiles(path, Array.from(blobs));

            if (workerfsMode == WORKERFS_MODE.JS) {
                if (typeof onWorkerFSMountReady == "function") {
                    onWorkerFSMountReady();
                }
            } else if (workerfsMode == WORKERFS_MODE.CCODE) {
                if (Module._onWorkerFSMountReady) {
                    Module._onWorkerFSMountReady();
                }
            }
        } else if (cmd == "DATA_READY") {
            const param = msg.data.param;
            const size = msg.data.size;
            const opid = msg.data.opid;

            if (workerfsMode == WORKERFS_MODE.JS) {
                if (typeof onWorkerFSDataReady == "function") {
                    onWorkerFSDataReady(new Uint8Array(msg.data.buffer), size, opid, param);
                }
            } else if (workerfsMode == WORKERFS_MODE.CCODE) {
                const buff = allocWorkerFSDataBuff(size);
                if (buff != null) {
                    Module.HEAPU8.set(new Uint8Array(msg.data.buffer), buff);
                    if (Module._onWorkerFSDataReady) {
                        Module._onWorkerFSDataReady(buff, size, opid, param);
                    }
                    freeWorkerFSDataBuff(buff);
                } else {
                    console.warn("EMS: Failed to alloc file buffer");
                };
            }
        } else if (cmd == "SIZE_READY") {
            const param = msg.data.param;
            const size = msg.data.size;
            const opid = msg.data.opid;

            if (workerfsMode == WORKERFS_MODE.JS) {
                if (typeof onWorkerFSDataReady == "function") {
                    onWorkerFSDataReady(0, size, opid, param);
                }
            } else if (workerfsMode == WORKERFS_MODE.CCODE) {
                if (Module._onWorkerFSDataReady) {
                    Module._onWorkerFSDataReady(0, size, opid, param);
                }
            }
        } else if ((cmd == "DATA_ERROR") ||
            (cmd == "SIZE_ERROR")) {
            const param = msg.data.param;
            const opid = msg.data.opid;

            if (workerfsMode == WORKERFS_MODE.JS) {
                if (typeof onWorkerFSDataReady == "function") {
                    onWorkerFSDataReady(0, 0, opid, param);
                }
            } else if (workerfsMode == WORKERFS_MODE.CCODE) {
                if (Module._onWorkerFSDataReady) {
                    Module._onWorkerFSDataReady(0, 0, opid, param);
                }
            }
        } else {
            console.warn("Main module unknown workerfs cmd: ", cmd);
        }
    };

    if (isDummy) {
        workerfs.postMessage({
            cmd: "INIT_DUMMY",
        });
    } else {
        workerfs.postMessage({
            cmd: "INIT",
            binPath: getBinPath(),
        });
    }
}

function terminateWorkerFS() {
    if (workerfsRef > 0) {
        if (!--workerfsRef) {
            workerfs.terminate();
            delete workerfs;
            workerfs = null;

            if ((workerFSCache.smallPtr != null) &&
                (workerFSCache.smallPtr !== 0)) {
                Module._free(workerFSCache.smallPtr);
                workerFSCache.smallPtr = null;
                workerFSCache.smallSize = 0;
            }
        }
    }
}

function filterWorkerFSMountFiles(files) {
    var allowedExtensions = [
        ".dat",
        ".bin",
        ".vas",
        ".txd",
    ];

    var filesArray = Array.prototype.slice.call(files);

    return filesArray.filter(function (file) {
        var relativePath = file.webkitRelativePath;
        var slashCount = (relativePath.match(/\//g) || []).length;
        if (slashCount !== 1) {
            return false;
        }

        var name = file.name.toLowerCase();
        var isAllowed = false;
        for (var i = 0; i < allowedExtensions.length; i++) {
            var ext = allowedExtensions[i];
            if (name.indexOf(ext, name.length - ext.length) !== -1) {
                isAllowed = true;
                break;
            }
        }
        return isAllowed;
    });
}

function postWorkerFSMount(mountPath) {
    if (cacheWorkerFSIsExist(mountPath)) {
        workerfs.postMessage({
            cmd: "MOUNT",
            blobs: cacheWorkerFSGetFiles(mountPath),
            path: mountPath,
        });
        return;
    }

    input = document.getElementById("in-directory");

    input.onchange = function () {
        const files = this.files;

        if (!files || (files.length === 0)) {
            return;
        }

        //console.log(`Selected directory: ${directoryPath}`);
        //console.log(`Found ${files.length} files:`);
        //for (let i = 0; i < files.length; i++) {
        //    console.log(`  - ${files[i].webkitRelativePath} (${files[i].size} bytes)`);
        //}

        const filesFiltered = filterWorkerFSMountFiles(files);

        var blobs = [];
        for (var i = 0; i < filesFiltered.length; i++) {
            var file = filesFiltered[i];
            blobs.push({
                name: file.name,
                data: file
            });
        }

        workerfs.postMessage({
            cmd: "MOUNT",
            blobs: blobs,
            path: mountPath,
        });

        this.value = "";
    };

    input.oncancel = function () {
        if (workerfsMode == WORKERFS_MODE.JS) {
            if (typeof onWorkerFSMountCancel == "function") {
                onWorkerFSMountCancel();
            }
        } else if (workerfsMode == WORKERFS_MODE.CCODE) {
            if (Module._onWorkerFSMountCancel) {
                Module._onWorkerFSMountCancel();
            }
        }
    };

    document.getElementById("in-directory").click();
}

function postWorkerFSRead(fname, offset, size, opid, param) {
    workerfs.postMessage({
        cmd: "READ",
        fname: fname,
        offset: offset,
        size: size,
        opid: opid,
        param: param,
    });
}

function postWorkerFSGetSize(fname, opid, param) {
    workerfs.postMessage({
        cmd: "SIZE",
        fname: fname,
        opid: opid,
        param: param,
    });
}

function cacheWorkerFSAddFiles(path, blobs) {
    workerfsMountCache[path] = blobs;
}

function cacheWorkerFSRemFiles(path) {
    delete workerfsMountCache[path];
}

function cacheWorkerFSGetFiles(path) {
    return workerfsMountCache[path];
}

function cacheWorkerFSIsExist(path) {
    return cacheWorkerFSGetFiles(path) != null;
}

function cacheWorkerFSReset() {
    workerfsMountCache = {};
}
