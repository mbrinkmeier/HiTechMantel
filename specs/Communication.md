# Kommunikation

## Nextion zu Master (Manschette)

Die Nachrichten haben die folgende Form:

    <START_BYTE> <TARGET_ID> <CMD_ID> <DLEN> <DATA_1> ... <DATA_DLEN>

- `<START_BYTE>` ist der Wert 42 (0x2A)
- `<TARGET_ID>` ist die I2C Id des Ziels Der Mater hat die ID 0
- `<CMD_ID>` ist die ID des jeweiligen Kommandos
- `<DLEN>` ist die Anzahl der folgenden Datenbytes
- `<DATA_I>` ist das i-te Datenbyte

Nach einem empfangenen `<START_BYTE>` liest der Master
`<TARGET_ID>`, `<CMD_ID>` und `<DLEN>`

Falls *CMS_ID* = 0 ist, verarbeitet er die Nachricht selbst. In allen anderen Fällen, leitet er

    <CMD_ID> <DLEN> <DATA_1> ... <DATA_DLEN>

auf den I2C Bus an den Slave mit der ID `<TARGET_ID>` weiter.

### Code auf dem Nextion um 4 Byte zu senden

Um vom Nextion eine 4 Byte Nachricht zu senden, kann sie auf
die folgende Art konstruiert werden:

    sys0=<BYTE3>
    sys0=sys0*256
    sys0=sys0+<BYTE2>
    sys0=sys0*256
    sys0=sys0+<BYTE1>
    sys0=sys0*256
    sys0=sys0+<BYTE0>
    print sys0

Dabei ist <BTEi> das jeweilige Byte von i = 0 bis 3.

## Die I2C Slaves

Die I2C Slaves empfangen Nachrichten der Form

    <CMD_ID> <DLEN> <DATA_1> ... <DATA_LEN>

Sie verarbeiten sie und leeren den I2C Puffer der
Transmission.
