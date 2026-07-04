
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>UCCPocker — Dou Dizhu Desktop Client</title>
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif; line-height: 1.6; color: #24292f; max-width: 960px; margin: 0 auto; padding: 24px; background: #fff; }
    h1, h2, h3 { line-height: 1.25; border-bottom: 1px solid #d0d7de; padding-bottom: 0.3em; }
    h1 { font-size: 2em; border-bottom-width: 2px; }
    h2 { font-size: 1.5em; margin-top: 2em; }
    h3 { font-size: 1.15em; margin-top: 1.5em; border-bottom: none; }
    code, pre { font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, monospace; font-size: 0.9em; }
    pre { background: #f6f8fa; border: 1px solid #d0d7de; border-radius: 6px; padding: 16px; overflow-x: auto; }
    code { background: #f6f8fa; padding: 0.2em 0.4em; border-radius: 4px; }
    pre code { background: none; padding: 0; }
    table { border-collapse: collapse; width: 100%; margin: 1em 0; }
    th, td { border: 1px solid #d0d7de; padding: 8px 12px; text-align: left; }
    th { background: #f6f8fa; font-weight: 600; }
    ul, ol { padding-left: 1.5em; }
    li { margin: 0.35em 0; }
    a { color: #0969da; text-decoration: none; }
    a:hover { text-decoration: underline; }
    .badge { display: inline-block; background: #ddf4ff; color: #0969da; border: 1px solid #54aeff66; border-radius: 2em; padding: 0.15em 0.75em; font-size: 0.85em; margin-right: 0.35em; margin-bottom: 0.35em; }
    .note { background: #fff8c5; border: 1px solid #d4a72c66; border-radius: 6px; padding: 12px 16px; margin: 1em 0; }
    .warn { background: #ffebe9; border: 1px solid #ff818266; border-radius: 6px; padding: 12px 16px; margin: 1em 0; }
    hr { border: none; border-top: 1px solid #d0d7de; margin: 2em 0; }
  </style>
</head>
<body>

<h1>UCCPocker</h1>

<p>
  <span class="badge">Qt 5.15</span>
  <span class="badge">C++17</span>
  <span class="badge">Windows</span>
  <span class="badge">Dou Dizhu</span>
  <span class="badge">WebSocket</span>
</p>

<p>
  <strong>UCCPocker</strong> is a desktop client for <em>Dou Dizhu</em> (Fight the Landlord), China's popular three-player trick-taking card game.
  It is a native Qt rewrite of the original Egret/TypeScript reference project in the <code>Check/</code> directory, extended with a full offline AI mode,
  online multiplayer, friend rooms, card animations, and a polished game table UI.
</p>

<hr>

<h2>Table of Contents</h2>
<ol>
  <li><a href="#features">Features</a></li>
  <li><a href="#tech-stack">Tech Stack</a></li>
  <li><a href="#project-structure">Project Structure</a></li>
  <li><a href="#requirements">Requirements</a></li>
  <li><a href="#getting-started">Getting Started</a></li>
  <li><a href="#running-tests">Running Tests</a></li>
  <li><a href="#configuration">Configuration</a></li>
  <li><a href="#online-multiplayer">Online Multiplayer</a></li>
  <li><a href="#asset-pipeline">Asset Pipeline</a></li>
  <li><a href="#architecture">Architecture</a></li>
  <li><a href="#keyboard-shortcuts">Keyboard Shortcuts</a></li>
  <li><a href="#reference-implementation">Reference Implementation</a></li>
  <li><a href="#license">License</a></li>
</ol>

<hr>

<h2 id="features">Features</h2>

<h3>Game Modes</h3>
<ul>
  <li><strong>Standalone (Offline)</strong> — Play against two AI opponents with full bidding and play flow.</li>
  <li><strong>Classic (Online)</strong> — Connect to a WebSocket game server for matchmaking and live play.</li>
  <li><strong>Friend Room</strong> — Create or join a private room by room ID.</li>
</ul>

<h3>Core Gameplay</h3>
<ul>
  <li>Standard 54-card deck; 17 cards per player + 3 landlord bottom cards.</li>
  <li>Call / pass / grab landlord bidding (<code>LandlordBidEngine</code>).</li>
  <li>All major Dou Dizhu hand types: singles, pairs, triples, straights, consecutive pairs, planes (with kicker variants), four-with-two, bombs, and king bomb.</li>
  <li>Play validation, comparison logic, and AI hint / auto-play seek engine.</li>
  <li>15-second turn countdown; pass / play / hint controls.</li>
</ul>

<h3>User Interface</h3>
<ul>
  <li>1280×720 logical stage scaled to any window size via <code>QGraphicsView</code>.</li>
  <li>Right-to-left card stacking (rightmost card on top), matching the original Egret layout.</li>
  <li>Unified paint-based hand widget — reliable Z-order and selection lift without ghosting.</li>
  <li>Card fly animations, bomb / king-bomb visual and sound effects.</li>
  <li>Landlord badges, opponent card counts, bottom-card reveal, and result dialog.</li>
  <li>Optional 2× / 3× supersampling for high-DPI / 4K displays.</li>
  <li>Fullscreen mode (F11) with Esc to exit.</li>
  <li>Application icon from <code>image/poker.png</code>.</li>
</ul>

<h3>Networking</h3>
<ul>
  <li>JSON-over-WebSocket protocol compatible with the reference server (<code>RoomProtocol</code>).</li>
  <li>Auto-reconnect with configurable retry count.</li>
  <li>Offline message queue flushed after reconnect.</li>
  <li>Reconnect state restore (hand, landlord, table cards, turn).</li>
  <li>Server connection test from the Settings dialog.</li>
</ul>

<h3>Audio</h3>
<ul>
  <li>Deal, discard, win/lose, click, bomb, and king-bomb sound effects.</li>
  <li>Background music (toggle in Settings).</li>
</ul>

<hr>

<h2 id="tech-stack">Tech Stack</h2>

<table>
  <thead>
    <tr><th>Component</th><th>Technology</th></tr>
  </thead>
  <tbody>
    <tr><td>Language</td><td>C++17</td></tr>
    <tr><td>UI Framework</td><td>Qt 5.15 (Widgets, Multimedia, Network, WebSockets)</td></tr>
    <tr><td>Build System</td><td>qmake + MinGW 64-bit (tested on Windows)</td></tr>
    <tr><td>Resources</td><td>Qt Resource System (<code>pocker.qrc</code>)</td></tr>
    <tr><td>Unit Tests</td><td>Qt Test (<code>tests/</code>)</td></tr>
    <tr><td>Asset Tools</td><td>Python 3 (<code>tools/</code>)</td></tr>
    <tr><td>Reference</td><td>Egret Engine TypeScript client (<code>Check/</code>)</td></tr>
  </tbody>
</table>

<hr>

<h2 id="project-structure">Project Structure</h2>

<pre><code>UCCPocker/
├── main.cpp / mainwindow.cpp     # Application entry & scene navigation
├── UCCPocker.pro                 # Main qmake project file
├── pocker.qrc                    # Bundled images, audio, config
├── image/                        # Game assets (cards, UI, sounds)
├── core/                         # Game logic (no Qt Widgets dependency)
│   ├── pokerutils.*              # Deck, sort, rank/suit helpers
│   ├── pokertypeutils.*          # Hand-type detection
│   ├── pokercompareutils.*       # Beat / compare logic
│   ├── pokerseekutils.*          # Hint & AI card search
│   ├── landlordbidengine.*       # Bidding state machine
│   ├── roomprotocol.h            # WebSocket message IDs
│   ├── appsettings.*             # Persistent user settings
│   ├── resourcemanager.*         # Pixmap / card image cache
│   └── soundmanager.*            # Audio playback
├── network/
│   └── gamewebsocketclient.*     # WebSocket client + reconnect queue
├── ui/                           # Qt Widgets presentation layer
│   ├── cardhandwidget.*          # Paint-based hand display
│   ├── cardlayout.*              # Fan / compact layout engine
│   ├── cardwidget.*              # Single-card drawing utilities
│   ├── gametableview.*           # Table layout (seats, play areas)
│   ├── standalonegamewidget.*    # Offline game mode
│   ├── classicgamewidget.*       # Online / friend-room mode
│   ├── gamehall.* / startscene.* # Navigation screens
│   ├── cardflyanimator.*         # Flying card animation
│   ├── boomeffectanimator.*      # Bomb effect overlay
│   └── supersamplegraphicsview.* # Optional supersampled rendering
├── tests/                        # Qt unit tests
├── tools/                        # Asset import & qrc generation scripts
└── Check/                        # Original Egret/TS reference (read-only)
</code></pre>

<hr>

<h2 id="requirements">Requirements</h2>

<h3>Build &amp; Run</h3>
<ul>
  <li><strong>OS:</strong> Windows 10/11 (primary target; Qt is cross-platform but this repo is configured for MinGW on Windows)</li>
  <li><strong>Qt:</strong> 5.15.2 or compatible 5.15.x with modules: <code>core</code>, <code>gui</code>, <code>widgets</code>, <code>multimedia</code>, <code>network</code>, <code>websockets</code></li>
  <li><strong>Compiler:</strong> MinGW 64-bit shipped with Qt (e.g. MinGW 8.1)</li>
  <li><strong>Python 3</strong> (optional) — for asset sync / qrc regeneration</li>
</ul>

<h3>Online Play</h3>
<ul>
  <li>A compatible Dou Dizhu WebSocket server (default: <code>ws://132.232.2.225:8800/pokerWebSocket</code> from the reference deployment)</li>
</ul>

<hr>

<h2 id="getting-started">Getting Started</h2>

<h3>1. Clone the Repository</h3>
<pre><code>git clone https://github.com/&lt;your-org&gt;/UCCPocker.git
cd UCCPocker
</code></pre>

<h3>2. Sync Assets (Optional)</h3>
<p>If card images or audio are missing, import them from the Egret reference or generate fallbacks:</p>
<pre><code>python tools/sync_assets.py
# or import from a custom folder:
python tools/sync_assets.py path/to/original_assets
# force regenerate placeholder assets:
python tools/sync_assets.py --generate
</code></pre>

<h3>3. Generate Application Icon (Windows)</h3>
<p>The Windows executable embeds <code>image/poker.ico</code>. If the ICO file is missing, create it from <code>image/poker.png</code>:</p>
<pre><code>python -c "from PIL import Image; img=Image.open('image/poker.png'); img.save('image/poker.ico', sizes=[(256,256),(128,128),(64,64),(48,48),(32,32),(16,16)])"
</code></pre>
<p>Requires <code>Pillow</code>: <code>pip install Pillow</code></p>

<h3>4. Configure Qt Environment</h3>
<p>Add Qt and MinGW to your <code>PATH</code> (adjust paths to your installation):</p>
<pre><code># PowerShell example
$env:PATH = "C:\Qt515\5.15.2\mingw81_64\bin;C:\Qt515\Tools\mingw810_64\bin;" + $env:PATH
</code></pre>

<h3>5. Build (Release)</h3>
<pre><code>mkdir build\release
cd build\release
qmake ..\..\UCCPocker.pro -spec win32-g++ "CONFIG+=release"
mingw32-make -j4
</code></pre>
<p>Output executable:</p>
<pre><code>build\release\release\UCCPocker.exe
</code></pre>

<h3>6. Run</h3>
<pre><code>.\build\release\release\UCCPocker.exe
</code></pre>

<div class="note">
  <strong>Note:</strong> Qt DLLs must be on <code>PATH</code>, or deploy with <code>windeployqt</code>:
  <pre><code>windeployqt build\release\release\UCCPocker.exe</code></pre>
</div>

<h3>Build with Qt Creator</h3>
<ol>
  <li>Open <code>UCCPocker.pro</code> in Qt Creator.</li>
  <li>Select the <strong>Desktop Qt 5.15.x MinGW 64-bit</strong> kit.</li>
  <li>Build → Run (Ctrl+R).</li>
</ol>

<hr>

<h2 id="running-tests">Running Tests</h2>

<pre><code>cd tests
qmake tests.pro -spec win32-g++ "CONFIG+=release"
mingw32-make -j4
release\UCCPockerTests.exe
</code></pre>

<p>Test suites cover:</p>
<ul>
  <li><code>PokerTypeUtils</code> — hand-type classification</li>
  <li><code>PokerCompareUtils</code> — beating logic between hands</li>
  <li><code>PokerSeekUtils</code> — hint / search algorithms</li>
  <li><code>LandlordBidEngine</code> — bidding state transitions</li>
</ul>

<hr>

<h2 id="configuration">Configuration</h2>

<p>Settings are stored via <code>QSettings</code> under organization <code>UCCPocker</code> (Windows Registry or platform equivalent). Configure in-game via <strong>Settings</strong>, or edit programmatic defaults in <code>core/appsettings.cpp</code>.</p>

<table>
  <thead>
    <tr><th>Setting</th><th>Default</th><th>Description</th></tr>
  </thead>
  <tbody>
    <tr><td>Player name</td><td><code>玩家</code></td><td>Display name in online modes</td></tr>
    <tr><td>Server URL</td><td><code>ws://132.232.2.225:8800/pokerWebSocket</code></td><td>WebSocket endpoint (player name appended as query param)</td></tr>
    <tr><td>Sound</td><td>On</td><td>Enable / disable audio</td></tr>
    <tr><td>Fullscreen</td><td>Off</td><td>Launch in fullscreen</td></tr>
    <tr><td>Supersample</td><td>1×</td><td>Render quality: 1×, 2×, or 3×</td></tr>
    <tr><td>Auto reconnect</td><td>On</td><td>Reconnect after disconnect</td></tr>
    <tr><td>Reconnect attempts</td><td>5</td><td>Maximum retry count</td></tr>
  </tbody>
</table>

<hr>

<h2 id="online-multiplayer">Online Multiplayer</h2>

<h3>Protocol Overview</h3>
<p>Client and server communicate with JSON messages identified by integer <code>type</code> fields defined in <code>core/roomprotocol.h</code>.</p>

<table>
  <thead>
    <tr><th>Direction</th><th>Code</th><th>Purpose</th></tr>
  </thead>
  <tbody>
    <tr><td>Request</td><td>1001</td><td>Enter room</td></tr>
    <tr><td>Request</td><td>1002</td><td>Ready</td></tr>
    <tr><td>Request</td><td>1003</td><td>Call / pass / grab landlord</td></tr>
    <tr><td>Request</td><td>1004</td><td>Discard (play or pass)</td></tr>
    <tr><td>Request</td><td>1005–1006</td><td>Create / join friend room</td></tr>
    <tr><td>Request</td><td>1007</td><td>Reconnect</td></tr>
    <tr><td>Response</td><td>2003</td><td>Deal cards</td></tr>
    <tr><td>Response</td><td>2005–2007</td><td>Landlord, discard, game over</td></tr>
    <tr><td>Response</td><td>2010</td><td>Reconnect snapshot</td></tr>
  </tbody>
</table>

<div class="warn">
  <strong>Server required:</strong> Online and friend-room modes need a running backend compatible with the reference Egret server protocol. The standalone mode works fully offline without any server.
</div>

<hr>

<h2 id="asset-pipeline">Asset Pipeline</h2>

<table>
  <thead>
    <tr><th>Script</th><th>Purpose</th></tr>
  </thead>
  <tbody>
    <tr><td><code>tools/sync_assets.py</code></td><td>Import assets from <code>Check/resource</code> or <code>original_assets/</code>, regenerate <code>pocker.qrc</code></td></tr>
    <tr><td><code>tools/generate_assets.py</code></td><td>Generate fallback card faces and placeholders</td></tr>
    <tr><td><code>tools/generate_qrc.py</code></td><td>Rebuild the Qt resource file from <code>image/</code></td></tr>
    <tr><td><code>tools/import_original_assets.py</code></td><td>One-shot import helper</td></tr>
  </tbody>
</table>

<p>After changing assets, rebuild the project so <code>rcc</code> recompiles <code>pocker.qrc</code>.</p>

<hr>

<h2 id="architecture">Architecture</h2>

<h3>Layered Design</h3>
<ul>
  <li><strong>core/</strong> — Pure game rules, card logic, bidding, settings. No widget code.</li>
  <li><strong>network/</strong> — WebSocket transport, reconnect, message queue.</li>
  <li><strong>ui/</strong> — Qt Widgets views, animations, table layout, mode-specific controllers.</li>
</ul>

<h3>Scene Flow</h3>
<pre><code>StartScene → GameHall → StandaloneGameWidget   (offline)
                     → ClassicGameWidget       (online classic)
                     → ClassicGameWidget       (friend create/join)
</code></pre>

<p>The main window embeds a fixed 1280×720 stage inside a <code>QGraphicsView</code> with optional supersampling. Page switches call <code>refreshStageLayout()</code> to reset proxy geometry and view cache.</p>

<h3>Card Display</h3>
<ul>
  <li><code>CardLayout</code> computes fan spacing and centering aligned with the Egret <code>PukerContainer</code> formulas.</li>
  <li><code>CardHandWidget</code> paints all cards in one <code>paintEvent</code> (index 0 bottom-left → index n−1 top-right).</li>
  <li>Selection only adjusts Y offset; Z-order never changes on select.</li>
</ul>

<hr>

<h2 id="keyboard-shortcuts">Keyboard Shortcuts</h2>

<table>
  <thead>
    <tr><th>Key</th><th>Action</th></tr>
  </thead>
  <tbody>
    <tr><td><kbd>F11</kbd></td><td>Toggle fullscreen</td></tr>
    <tr><td><kbd>Esc</kbd></td><td>Exit fullscreen</td></tr>
  </tbody>
</table>

<hr>

<h2 id="reference-implementation">Reference Implementation</h2>

<p>
  The <code>Check/</code> directory contains the original Egret/TypeScript Dou Dizhu client.
  UCCPocker ports its card layout rules, protocol constants, and asset structure while adding:
</p>
<ul>
  <li>Complete offline AI and bidding (partially stubbed in the TS version)</li>
  <li>Functional classic and friend-room online UI</li>
  <li>Native desktop performance, supersampling, and fullscreen</li>
  <li>Unit tests for core card logic</li>
</ul>

<hr>

<h2 id="license">License</h2>

<p>
  This project includes art and audio assets derived from the Egret reference client in <code>Check/</code>.
  Please review the original project and asset licenses before redistribution.
  Add an explicit <code>LICENSE</code> file to this repository if you publish it publicly.
</p>

<hr>

<p><em>UCCPocker — A Qt desktop client for Dou Dizhu (Fight the Landlord).</em></p>

</body>
</html>
