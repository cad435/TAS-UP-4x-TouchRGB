/**
 * ETS Script: Reads raw CAP1188 delta counts from the device via KNX bus.
 *
 * Called by the "Delta Counts auslesen" button in ETS (Touch Sensing page).
 * Uses FunctionProperty to communicate with the TDD_Module on the device.
 *
 * Protocol:
 *   Request:  [0]                              — Command byte 0 = read delta counts
 *   Response: [status, padA, padB, padC, padD]  — Status (0=OK) + 4x int8 as unsigned bytes
 *
 * FunctionProperty addressing:
 *   objectIndex = 161 (TDD_Module, after 158=Common, 159=FileTransfer, 160=Logic)
 *   propertyId  = 1
 *
 * The delta count values represent the difference between the current capacitive
 * sensor reading and the baseline. Positive = touch detected, negative = no touch.
 * These values are useful for tuning touch/proximity thresholds.
 *
 * @param {object} device   - ETS device object, provides access to device parameters
 * @param {object} online   - KNX bus connection for FunctionProperty calls
 * @param {object} progress - UI progress indicator (setText to show status)
 * @param {object} context  - ETS script context (unused)
 */
function TTD_readDeltaCounts(device, online, progress, context) {
    progress.setText("Lese Delta Counts vom Touch-Sensor...");

    // Open a connection-oriented (tunneling) connection to the device
    online.connect();

    // Send command byte 0 = "read delta counts" via FunctionProperty
    var data = [0];
    var resp = online.invokeFunctionProperty(161, 1, data); // objectIndex=161, propertyId=1

    online.disconnect();

    if (resp[0] == 0) { // Status byte 0 = success
        // Convert unsigned bytes (0-255) back to signed int8 (-128 to +127).
        // The firmware casts int8_t to uint8_t for transmission; we reverse that here.
        var dA = resp[1] > 127 ? resp[1] - 256 : resp[1]; // Pad A delta count
        var dB = resp[2] > 127 ? resp[2] - 256 : resp[2]; // Pad B delta count
        var dC = resp[3] > 127 ? resp[3] - 256 : resp[3]; // Pad C delta count
        var dD = resp[4] > 127 ? resp[4] - 256 : resp[4]; // Pad D delta count

        var result = "A:" + dA + " B:" + dB + " C:" + dC + " D:" + dD;
        progress.setText("Delta Counts: " + result);

        // Write the result string into the display-only ETS parameter.
        // This updates the ParameterSeparator text on the Touch Sensing page.
        // Parameter naming convention: PREFIX_Name → "TTD_DeltaCountResult"
        var par = device.getParameterByName("TTD_DeltaCountResult");
        par.value = result;
    } else {
        throw new Error("Fehler beim Lesen der Delta Counts (Status: " + resp[0] + ")");
    }
}
