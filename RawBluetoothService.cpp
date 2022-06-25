#include "MicroBitConfig.h"
#include "RawBluetoothService.h"
#include "MicroBitEvent.h"

//================================================================
#if MICROBIT_CODAL
//================================================================

const uint8_t RawBluetoothService::service_base_uuid[16] = {
    0xec, 0x21, 0x00, 0x00, 0x20, 0x69, 0x30, 0xda, 0xa2, 0x6e, 0x12, 0x73, 0xcb, 0x04, 0x83, 0x49
};

const uint8_t RawBluetoothService::char_base_uuid[16] = {
    0xec, 0x21, 0x00, 0x00, 0x20, 0x69, 0x30, 0xda, 0xa2, 0x6e, 0x12, 0x73, 0xcb, 0x04, 0x83, 0x4a
};

const uint16_t RawBluetoothService::serviceUUID = 0xa0c0;
const uint16_t RawBluetoothService::charUUID[mbbs_cIdxCOUNT] = {0xbcc1, 0xbcc2};

RawBluetoothService::RawBluetoothService(BLEDevice &_ble) : ble(_ble)
{
    // Initialise our characteristic values.
    memset(&rxBuffer, 0, sizeof(rxBuffer));
    memset(&txBuffer, 0, sizeof(txBuffer));

    receivedBytes = 0;

    // Register the base UUID and create the service.
    RegisterBaseUUID(service_base_uuid);
    CreateService(serviceUUID);

    RegisterBaseUUID(char_base_uuid);
    CreateCharacteristic(
        mbbs_cIdxRX, charUUID[mbbs_cIdxRX],
        (uint8_t *)&rxBuffer, 1,
        sizeof(rxBuffer), microbit_propWRITE);

    CreateCharacteristic(
        mbbs_cIdxTX, charUUID[mbbs_cIdxTX],
        (uint8_t *)&txBuffer, 1,
        sizeof(txBuffer), microbit_propNOTIFY
    );
}

void RawBluetoothService::write(const uint8_t *data, uint8_t len)
{
    if (getConnected())
    {
        len = len > RAWBLUETOOTH_DATA_LENGTH ? RAWBLUETOOTH_DATA_LENGTH : len;
        memcpy(&txBuffer, data, len);
        notifyChrValue(mbbs_cIdxTX, (uint8_t *)&txBuffer, len);
    }
}

uint8_t RawBluetoothService::read(uint8_t *data)
{
    uint8_t len = receivedBytes;
    memcpy(data, &rxBuffer, len);
    receivedBytes = 0;
    return len;
}

/**
 * A callback function for whenever a Bluetooth device writes to our RX characteristic.
 */
void RawBluetoothService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle(mbbs_cIdxRX) && params->len > 0)
    {
        receivedBytes = params->len;
        memcpy(&rxBuffer, params->data, params->len);
        MicroBitEvent(MICROBIT_ID_RAWBLUETOOTH, MICROBIT_RAWBLUETOOTH_EVT_RX);
    }
}

//================================================================
#else // MICROBIT_CODAL
//================================================================

#include "ble/UUID.h"

RawBluetoothService::RawBluetoothService(BLEDevice &_ble) : ble(_ble)
{
    GattCharacteristic rxCharacteristic(
        RawBluetoothRxCharacteristicUUID, (uint8_t *)&rxBuffer, 0,
        sizeof(rxBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    GattCharacteristic txCharacteristic(
        RawBluetoothTxCharacteristicUUID, (uint8_t *)&txBuffer, 0,
        sizeof(txBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    // Initialise our characteristic values.
    memset(&rxBuffer, 0, sizeof(rxBuffer));
    memset(&txBuffer, 0, sizeof(txBuffer));

    receivedBytes = 0;

    // Set default security requirements
    rxCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    txCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    // setup GATT table
    GattCharacteristic *characteristics[] = {&rxCharacteristic, &txCharacteristic};
    GattService service(RawBluetoothServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));
    ble.addService(service);

    // retreive handles
    rxCharacteristicHandle = rxCharacteristic.getValueHandle();
    txCharacteristicHandle = txCharacteristic.getValueHandle();

    // initialize data
    ble.gattServer().write(txCharacteristicHandle, (uint8_t *)&txBuffer, sizeof(txBuffer));
    ble.gattServer().onDataWritten(this, &RawBluetoothService::onDataWritten);
}

void RawBluetoothService::write(const uint8_t *data, uint8_t len)
{
    if (ble.getGapState().connected)
    {
        len = len > RAWBLUETOOTH_DATA_LENGTH ? RAWBLUETOOTH_DATA_LENGTH : len;
        memcpy(&txBuffer, data, len);
        ble.gattServer().notify(txCharacteristicHandle, (uint8_t *)&txBuffer, len);
    }
}

uint8_t RawBluetoothService::read(uint8_t *data)
{
    uint8_t len = receivedBytes;
    memcpy(data, &rxBuffer, len);
    receivedBytes = 0;
    return len;
}

/**
 * A callback function for whenever a Bluetooth device writes to our RX characteristic.
 */
void RawBluetoothService::onDataWritten(const GattWriteCallbackParams *params)
{
    if (params->handle == this->rxCharacteristicHandle && params->len > 0)
    {
        receivedBytes = params->len;
        memcpy(&rxBuffer, params->data, params->len);
        MicroBitEvent(MICROBIT_ID_RAWBLUETOOTH, MICROBIT_RAWBLUETOOTH_EVT_RX);
    }
}

const uint8_t RawBluetoothServiceUUID[] = {
    0xec, 0x21, 0xa0, 0xc0, 0x20, 0x69, 0x30, 0xda, 0xa2, 0x6e, 0x12, 0x73, 0xcb, 0x04, 0x83, 0x49
};

const uint8_t RawBluetoothRxCharacteristicUUID[] = {
    0xec, 0x21, 0xbc, 0xc1, 0x20, 0x69, 0x30, 0xda, 0xa2, 0x6e, 0x12, 0x73, 0xcb, 0x04, 0x83, 0x4a
};

const uint8_t RawBluetoothTxCharacteristicUUID[] = {
    0xec, 0x21, 0xbc, 0xc2, 0x20, 0x69, 0x30, 0xda, 0xa2, 0x6e, 0x12, 0x73, 0xcb, 0x04, 0x83, 0x4a
};

//================================================================
#endif // MICROBIT_CODAL
//================================================================
