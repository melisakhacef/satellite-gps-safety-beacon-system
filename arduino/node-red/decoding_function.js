// Vérifier que frmPayload existe
if (!msg.payload || !msg.payload.frmPayload) {
    return null;  // ignore message
}

// Base64 → buffer ASCII
let buf = Buffer.from(msg.payload.frmPayload, 'base64');

// ASCII → HEX string
let hexString = buf.toString('utf8');

// Vérifier longueur minimum (16 caractères HEX = 8 octets)
if (!hexString || hexString.length < 16) {
    node.warn("Payload trop court : " + hexString);
    return null;
}

// HEX string → vrai buffer binaire
let realBuf = Buffer.from(hexString, 'hex');

// Sécurité supplémentaire
if (realBuf.length < 8) {
    node.warn("Buffer trop court après conversion");
    return null;
}

// Lecture sécurisée
let longitude = realBuf.readInt32BE(0) / 1e6;
let latitude = realBuf.readInt32BE(4) / 1e6;

// ==============================
// AJOUT CALCUL DISTANCE
// ==============================

function toRad(x) {
    return x * Math.PI / 180;
}

let lastPoint = flow.get("lastPoint");
let totalDistance = flow.get("totalDistance") || 0;

if (lastPoint) {

    const R = 6371000; // rayon Terre en mètres

    let dLat = toRad(latitude - lastPoint.latitude);
    let dLon = toRad(longitude - lastPoint.longitude);

    let a =
        Math.sin(dLat / 2) * Math.sin(dLat / 2) +
        Math.cos(toRad(lastPoint.latitude)) *
        Math.cos(toRad(latitude)) *
        Math.sin(dLon / 2) * Math.sin(dLon / 2);

    let c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    let distance = R * c;

    totalDistance += distance;
}

// Sauvegarde dernier point
flow.set("lastPoint", { latitude, longitude });
flow.set("totalDistance", totalDistance);

// ==============================
// Payload final
// ==============================

msg.payload = {
    latitude: latitude,
    longitude: longitude,
    distance_total_m: totalDistance
};

return msg;
