var Module = {};

function getFS() {
    return self.Module["FS"];
}

function getWORKERFS(fs) {
    return fs.filesystems.WORKERFS;
}

function onInitDummy(data) {
    self.Module = {
        instantiateWasm: function (imports, successCallback) {
            return {};
        },

        getPreloadedPackage: function (name, size) {
            return new ArrayBuffer(0);
        },

        onRuntimeInitialized: function () {
            self.postMessage({
                cmd: "INIT_DUMMY_READY"
            });
        }
    };

    self.importScripts("./../index.js");
}

function onInit(data) {
    const binPath = data.binPath

    self.Module = {
        locateFile: function (path, prefix) {
            if (path.endsWith(".data") ||
                path.endsWith(".wasm")) {
                return "./../" + binPath + path;
            }

            return "./../" + prefix + path;
        },

        onRuntimeInitialized: function () {
            self.postMessage({
                cmd: "INIT_READY"
            });
        }
    };

    self.importScripts("./../" + binPath + "index.js");
}

function onMount(data) {
    const FS = getFS();
    const WORKERFS = getWORKERFS(FS);
    
    const blobs = data.blobs;
    const path = data.path;

    FS.mkdir(path);
    FS.mount(WORKERFS, { blobs: blobs }, path);

    self.postMessage({
        cmd: "MOUNT_READY",
        blobs: blobs,
        path: path,
    })
}

function onRead(data) {
    const FS = getFS();
    
    const fname = data.fname;
    const offset = data.offset;
    const param = data.param;
    const opid = data.opid;
    size = data.size;

    var path = (fname);

    var stream;
    try {
        stream = FS.open(path, "r");
    } catch (e) {
        self.postMessage({
            cmd: "DATA_ERROR",
            size: 0,
            opid: opid,
            param: param,
        });
        return;
    }

    var stat = FS.stat(path);
    if (stat.size < size)
        size = stat.size;

    if (size > (stat.size - offset))
        size = (stat.size - offset);

    var buffer = new Uint8Array(size);
    var bytesRead = FS.read(stream, buffer, 0, size, offset);
    FS.close(stream);

    self.postMessage({
        cmd: "DATA_READY",
        size: bytesRead,
        opid: opid,
        param: param,
        buffer: buffer.buffer,
    }, [buffer.buffer]);
}

function onSize(data) {
    const FS = getFS();
    const fname = data.fname;
    const param = data.param;
    const opid = data.opid;

    var path = (fname);
    var stream;
    try {
        stream = FS.open(path, "r");
    } catch (e) {
        self.postMessage({
            cmd: "SIZE_ERROR",
            size: 0,
            opid: opid,
            param: param,
        });
        return;
    }

    var stat = FS.stat(path);

    self.postMessage({
        cmd: "SIZE_READY",
        size: stat.size,
        opid: opid,
        param: param,
    });
}

self.onmessage = function (msg) {
    const cmd = msg.data.cmd;

    switch (cmd) {
        case "INIT_DUMMY":
            onInitDummy(msg.data);
            break;
        
        case "INIT":
            onInit(msg.data);
            break;
        
        case "MOUNT":
            onMount(msg.data);
            break;
        
        case "READ":
            onRead(msg.data);
            break;
        
        case "SIZE":
            onSize(msg.data);
            break;
        
        default:
            console.warn('Unknown workerfs command: ', cmd);
            break;
    }
}