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
  weapon: {
    defIndex: 0,
    name: 'No Weapon',
    type: 'none'
  },
  knife: {
    defIndex: 0,
    name: 'Default Knife'
  },
  gloves: {
    defIndex: 0,
    name: 'Default Gloves'
  },
  paintKit: 0,
  wear: 0.01,
  seed: 0,
  isApplied: false,
  lastUpdate: new Date().toISOString()
};

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(join(__dirname, 'public')));

// Load configuration
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

// Save configuration
function saveConfig(config) {
  try {
    writeFileSync(CONFIG_FILE, JSON.stringify(config, null, 2));
  } catch (e) {
    console.error('Failed to save config:', e);
  }
}

let skinConfig = loadConfig();
const clients = new Set();

// REST API Endpoints
app.get('/api/status', (req, res) => {
  res.json({
    status: 'active',
    config: skinConfig,
    timestamp: new Date().toISOString()
  });
});

app.get('/api/weapons', (req, res) => {
  const weapons = [
    // Pistols
    { id: 1, name: 'USP-S', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 2, name: 'P250', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 3, name: 'Five-SeveN', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 4, name: 'Tec-9', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 7, name: 'P90', type: 'smg', category: 'SMGs', icon: '🔫' },
    { id: 8, name: 'MAC-10', type: 'smg', category: 'SMGs', icon: '🔫' },
    { id: 9, name: 'MP9', type: 'smg', category: 'SMGs', icon: '🔫' },
    { id: 10, name: 'MP7', type: 'smg', category: 'SMGs', icon: '🔫' },
    { id: 19, name: 'Famas', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 20, name: 'Galil AR', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 21, name: 'AK-47', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 22, name: 'M4A4', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 23, name: 'M4A1-S', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 24, name: 'AUG', type: 'rifle', category: 'Rifles', icon: '🔭' },
    { id: 25, name: 'Glock-18', type: 'pistol', category: 'Pistols', icon: '🔫' },
    { id: 26, name: 'AWP Dragon Lore', type: 'sniper', category: 'Sniper Rifles', icon: '🎯' },
    { id: 38, name: 'M249', type: 'machine', category: 'Machine Guns', icon: '🔫' },
    { id: 40, name: 'Negev', type: 'machine', category: 'Machine Guns', icon: '🔫' }
  ];
  res.json(weapons);
});

app.get('/api/gloves', (req, res) => {
  const gloves = [
    { id: 0, name: 'No Gloves' },
    { id: 5027, name: 'Bloodhound Gloves' },
    { id: 5028, name: 'SpecGO Gloves' },
    { id: 5029, name: 'Superconductor Gloves' },
    { id: 5030, name: 'Yellow Gloves' },
    { id: 5031, name: 'Leather Gloves' },
    { id: 5032, name: 'Crimson Gloves' },
    { id: 5033, name: 'Midnight Gloves' }
  ];
  res.json(gloves);
});

app.get('/api/knives', (req, res) => {
  const knives = [
    { id: 0, name: 'Default Knife' },
    { id: 500, name: 'Bayonet' },
    { id: 505, name: 'Bowie Knife' },
    { id: 506, name: 'Butterfly Knife' },
    { id: 507, name: 'Falchion Knife' },
    { id: 508, name: 'Flip Knife' },
    { id: 509, name: 'Gut Knife' },
    { id: 512, name: 'Huntsman Knife' },
    { id: 514, name: 'M9 Bayonet' },
    { id: 515, name: 'Karambit' },
    { id: 517, name: 'Shadow Daggers' },
    { id: 518, name: 'Navaja Knife' },
    { id: 520, name: 'Ursus Knife' },
    { id: 521, name: 'Stiletto Knife' },
    { id: 522, name: 'Talon Knife' },
    { id: 523, name: 'StatTrak Bayonet' }
  ];
  res.json(knives);
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

  // Broadcast update to WebSocket clients
  broadcastUpdate({
    type: 'skin-applied',
    config: skinConfig
  });

  res.json({
    success: true,
    config: skinConfig
  });
});

app.post('/api/skin/reset', (req, res) => {
  skinConfig = { ...DEFAULT_CONFIG };
  saveConfig(skinConfig);

  broadcastUpdate({
    type: 'skin-reset',
    config: skinConfig
  });

  res.json({
    success: true,
    config: skinConfig
  });
});

// WebSocket server
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

function broadcastUpdate(data) {
  const message = JSON.stringify(data);
  clients.forEach(client => {
    if (client.readyState === 1) {
      client.send(message);
    }
  });
}

wss.on('connection', (ws) => {
  console.log('Client connected');
  clients.add(ws);

  // Send current config on connection
  ws.send(JSON.stringify({
    type: 'initial-state',
    config: skinConfig
  }));

  ws.on('close', () => {
    console.log('Client disconnected');
    clients.delete(ws);
  });

  ws.on('error', (err) => {
    console.error('WebSocket error:', err);
  });
});

server.listen(PORT, () => {
  console.log(`
╔════════════════════════════════════════════╗
║   CS2 Skin Changer - Web Interface v1.0   ║
╚════════════════════════════════════════════╝

🎮 Server running on http://localhost:${PORT}
📡 WebSocket ready for real-time updates
💾 Config saved to: ${CONFIG_FILE}

  `);
});
