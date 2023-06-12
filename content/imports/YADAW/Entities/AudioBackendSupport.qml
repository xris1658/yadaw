import QtQml

QtObject {
    enum Backend {
        Off,
        AudioGraph,
        ALSA
    }
    function isBackendSupported(backend) {
        return true;
    }
}