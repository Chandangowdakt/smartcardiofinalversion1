import express from 'express';
import http from 'http';
import cors from 'cors';
import dotenv from 'dotenv';
import mongoose from 'mongoose';
import { Server } from 'socket.io';

import authRoutes from './routes/authRoutes.js';
import dataRoutes from './routes/dataRoutes.js';
import reportRoutes from './routes/reportRoutes.js';
import initRealtime from './utils/realtime.js';

dotenv.config();

const app = express();

app.use(
  cors({
    origin: [
      'http://localhost:5173',
      'http://localhost:3000',
      process.env.FRONTEND_ORIGIN || '',
      process.env.FRONTEND_ORIGIN_2 || '',
    ].filter(Boolean),
    credentials: true,
  })
);

app.use(express.json());

app.get('/', (req, res) => res.send('SmartCardio backend running'));

app.use('/api/auth', authRoutes);
app.use('/api/data', dataRoutes);
app.use('/api/report', reportRoutes);

const httpServer = http.createServer(app);

const io = new Server(httpServer, {
  cors: {
    origin: '*',
  },
});

initRealtime(io);

const PORT = process.env.PORT || 5000;
const MONGO_URI = process.env.MONGO_URI || 'mongodb://127.0.0.1:27017/smartcardio';

mongoose
  .connect(MONGO_URI)
  .then(() => console.log('✅ MongoDB connected'))
  .catch((err) => console.error('MongoDB error:', err.message));

httpServer.listen(PORT, '0.0.0.0', () => {
  console.log(`🚀 Server running on http://0.0.0.0:${PORT}`);
});