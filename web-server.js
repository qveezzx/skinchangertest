import express from 'express';
import { WebSocketServer } from 'ws';
import cors from 'cors';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import { readFileSync, writeFileSync, existsSync } from 'fs';
import http from 'http';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = process.env.PORT || 3000;
const CONFIG_FILE = join(__dirname, 'skinconfig.json');

// Default skin configuration
const DEFAULT_CONFIG = {
  weapon: { defIndex: 0, name: 'No Weapon', type: 'none' },
  knife: { defIndex: 0, name: 'Default Knife' },
  gloves: { defIndex: 0, name: 'Default Gloves' },
  paintKit: 0,
  wear: 0.01,
  seed: 0,
  isApplied: false,
  lastUpdate: new Date().toISOString()
};

// --- Middleware ---
app.use(cors());
app.use(express.json());

// 1. If your HTML is in a folder called 'public', use this:
app.use(express.static(join(__dirname, 'public')));

// 2. ALSO allow serving files from the root folder (just in case)
app.use(express.static(__dirname));

// --- Fix: Explicit Root Route ---
app.get('/', (req, res) => {
  // Try to find index.html in /public first, then root
  const publicPath = join(__dirname, 'public', 'index.html');
  const rootPath = join(__dirname, 'index.html');
  
  if (existsSync(publicPath)) {
    res.sendFile(publicPath);
  } else if (existsSync(rootPath)) {
    res.sendFile(rootPath);
  } else {
    res.status(404).send('<h1>Error: index.html not found!</h1><p>Please ensure index.html exists in your project folder.</p>');
  }
});

// --- Config Logic ---
function loadConfig() {
  if (existsSync(CONFIG_FILE)) {
    try {
      return JSON.parse(readFileSync(CONFIG_FILE, 'utf8'));
    } catch (e) {
      console.error('Failed to load config:', e);
    }
  }
  return { ...DEFAULT_CONFIG };
}

function saveConfig(config) {
  try {
    writeFileSync(CONFIG_FILE, JSON.stringify(config, null, 2));
  } catch (e) {
    console.error('Failed to save config:', e);
  }
}

let skinConfig = loadConfig();
const clients = new Set();

// --- REST API Endpoints ---
app.get('/api/status', (req, res) => {
  res.json({ status: 'active', config: skinConfig, timestamp: new Date().toISOString() });
});

app.get('/api/weapons', (req, res) => {
  const weapons = [
    { id: 1, name: 'USP-S', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 21, name: 'AK-47', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 22, name: 'M4A4', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 23, name: 'M4A1-S', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 26, name: 'AWP', type: 'sniper', category: 'Sniper Rifles', icon: '🎯' }
    // ... add others as needed
  ];
  res.json(weapons);
});

app.post('/api/skin/apply', (req, res) => {
  const { weapon, knife, gloves, paintKit, wear, seed } = req.body;
  skinConfig = {
    ...skinConfig,
    weapon: weapon || skinConfig.weapon,
    knife: knife || skinConfig.knife,
    gloves: gloves || skinConfig.gloves,
    paintKit: paintKit ?? skinConfig.paintKit,
    wear: wear ?? skinConfig.wear,
    seed: seed ?? skinConfig.seed,
    isApplied: true,
    lastUpdate: new Date().toISOString()
  };
  saveConfig(skinConfig);
  broadcastUpdate({ type: 'skin-applied', config: skinConfig });
  res.json({ success: true, config: skinConfig });
});

// --- WebSocket Setup ---
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

function broadcastUpdate(data) {
  const message = JSON.stringify(data);
  clients.forEach(client => {
    if (client.readyState === 1) client.send(message);
  });
}

wss.on('connection', (ws) => {
  clients.add(ws);
  ws.send(JSON.stringify({ type: 'initial-state', config: skinConfig }));
  ws.on('close', () => clients.delete(ws));
});

server.listen(PORT, () => {
  console.log(`
╔════════════════════════════════════════════╗
║    CS2 Skin Changer - Web Interface v1.0   ║
╚════════════════════════════════════════════╝
🎮 Server running on http://localhost:${PORT}
📡 WebSocket ready for real-time updates
💾 Config: ${CONFIG_FILE}
  `);
});