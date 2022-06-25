const MICROBIT_ID_RAWBLUETOOTH = 2300;
const MICROBIT_RAWBLUETOOTH_EVT_RX = 60;

/**
 * Supports for raw bluetooth service.
 */
//% weight=50 icon="\uf294" color=#0055f4
namespace rawbluetooth {
    let onReceivedHandler: (bytes: number[]) => void = null;

    /**
     * Starts service.
     */
    //% blockId=rawbluetooth_start block="raw bluetooth start"
    //% weight=90 blockGap=8
    export function start(): void {
        startService();

        // on received
        control.onEvent(MICROBIT_ID_RAWBLUETOOTH, MICROBIT_RAWBLUETOOTH_EVT_RX, () => {
            const buffer = readBuffer();
            if (buffer.length > 0 && onReceivedHandler) {
                onReceivedHandler(buffer.toArray(NumberFormat.UInt8LE));
            }
        });
    }

    /**
     * Sends raw data buffer(from a byte array).
     */
    //% blockId=rawbluetooth_sendbuffer
    //% block="raw bluetooth|send buffer from %bytes"
    //% weight=80 blockGap=8
    export function sendBuffer(bytes: number[]): void {
        const buffer = Buffer.fromArray(bytes);
        writeBuffer(buffer);
    }

    /**
     * On received raw data.
     */
    //% blockId=rawbluetooth_onreceived
    //% block="on raw bluetooth received"
    //% weight=70 blockGap=32 draggableParameters
    export function onReceived(cb: (bytes: number[]) => void) {
        onReceivedHandler = cb;
    }

    /**
     * Returns true if the service is connected.
     */
    //% blockId=rawbluetooth_isconnected block="raw bluetooth is connected"
    //% weight=20 blockGap=8 shim=rawbluetooth::isConnected
    export function isConnected(): boolean {
        return false;
    }

    /**
     * Sets the bluetooth transmit power between 0 (minimal) and 7 (maximum).
     * @param power power level between 0 (minimal) and 7 (maximum), eg: 7.
     */
    //% weight=10
    //% blockId=rawbluetooth_settransmitpower block="raw bluetooth|set transmit power %power"
    //% power.min=0 power.max=7 shim=rawbluetooth::setTransmitPower
    //% blockGap=8
    export function setTransmitPower(power: number) {
        return;
    }

    /**
     * Starts the raw bluetooth service.
     */
    //% shim=rawbluetooth::startService
    function startService(): void {
        return;
    }

    /**
     * write buffer.
     */
    //% shim=rawbluetooth::writeBuffer
    function writeBuffer(buffer: Buffer): void {
        return;
    }

    /**
     * read buffer.
     */
    //% shim=rawbluetooth::readBuffer
    function readBuffer(): Buffer {
        return Buffer.create(0);
    }
}
