# SmartCardio Final Version (Backend + Frontend + ESP32)

This is a ready-to-run version of your SmartCardio project with:

- Email signup & login
- Forgot password with OTP (shown in backend console)
- Dashboard showing HR, BP (simulated), SpO2
- Sensor / WiFi / Backend status indicators
- Live ECG monitor via Socket.IO
- Alerts when HR / SpO2 are abnormal
- PDF report download with ECG snapshot
- ESP32 demo firmware that sends fake but realistic vitals via HTTP POST

---

## 1. Backend Setup

### Requirements
- Node.js (v18+ recommended)
- MongoDB running locally on default port

### Steps

```bash
cd backend
npm install
```

Create a `.env` file in `backend` folder (you can copy `.env.example`):

```env
MONGO_URI=mongodb://127.0.0.1:27017/smartcardio
JWT_SECRET=smartcardio_secret
FRONTEND_ORIGIN=http://localhost:5173
```

Run backend:

```bash
npm run dev
```

Backend runs on: `http://0.0.0.0:5000`

---

## 2. Frontend Setup

```bash
cd frontend
npm install
npm run dev
```

Frontend runs on: `http://localhost:5173`

It uses TailwindCSS + React + Vite.

If your backend is on another IP (for ESP32), update `src/utils/api.js`:

```js
export const API_BASE =
  import.meta.env.VITE_API_URL || "http://<YOUR_LAPTOP_IPV4>:5000";
```

---

## 3. ESP32 Firmware

Open `esp32/SmartCardio_ESP32_HTTP_Demo.ino` in Arduino IDE.

- Board: `ESP32 Dev Module`
- Change WiFi SSID & password to match your hotspot/router.
- Change `serverBase` to your laptop IPv4 + port 5000, e.g.:

```cpp
const char* serverBase = "http://192.168.137.1:5000";
```

Flash the code.

Every second, it will POST data to:

`http://<serverBase>/api/data/push`

Dashboard & Live Monitor will display it and generate alerts/PDF.

---

## 4. Flow Summary

ESP32  --HTTP POST-->  Backend (/api/data/push)
Backend --MongoDB--> saves Reading
Backend --Socket.IO--> sends realtime + alerts
Frontend:
  - Dashboard polls `/api/data/latest` + `/api/data/alerts`
  - LiveMonitor uses Socket.IO realtime ECG
  - PDF report via `/api/report/latest`

You can extend the ESP32 firmware later to use real AD8232 + MAX30102,
keeping the same JSON fields: `hr`, `spo2`, `sys`, `dia`, `ecg[]`, `ppg[]`.