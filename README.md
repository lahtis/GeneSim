<h1>🚀 What’s Actually New in v1.3.1?</h2>
<h2>💎 1. The Power of "The Seed"</h2>
In the demo, if you found a perfect character, they were gone the moment you closed the app.
<li>Now: By using the --seed flag, you can share a specific number with a teammate, and they will generate the exact same names and ages on their machine. This is vital for game developers who need to keep character identities consistent across different builds.</li>

<h2>📁 2. Pro-Grade Data Export</h2>
While the demo focused on "looking good" in the console, v1.3.1 focuses on Data Portability.
<li>Valid JSON: We fixed the critical schema bug where years were outside the objects. Now, you can pipe the output directly into a web app or a game engine like Unity or Unreal without any extra parsing code.</li>
<li>Clean CSV: A standardized 5-column structure ensures that your historical data opens perfectly in Excel or Google Sheets for world-building spreadsheets.</li>

<h2>🧬 3. Intelligent Chronology</h2>
The demo had "Period-Specific Realism," but v1.3.1 adds Logical Realism.
<li>The age generation is now hard-coded into the export logic. When you generate a family, the birth years are mathematically consistent with the selected historical period, ensuring no "time-traveling" ancestors.</li>

<h2>🧪 4. Automated Verification</h2>
The demo required manual testing.
<li>v1.3.0 comes with a Python Test Suite. You can now verify that your name files, memory allocation, and output formats are working correctly in seconds. This makes it a professional-grade "production asset."</li>

<h2>Summary: The "Swiss Army Knife" is Sharper</h2>
The demo was a character creator. Version 1.3.1 is a data engine. You have moved from a tool that shows you a name to a tool that delivers a validated, reproducible, and structured dataset.

<h2>Technical Specifications</h2>
<li>Language: Pure C.</li>
<li>Compatibility: Designed to work on both Windows and Linux environments (using #ifdef constructs).</li>
<li>Version: v2.0.0 (Development stage).</li>
<li>Data is based on https://www.tuomas.salste.net/suku/kela-etunimet.html#M1 research. There are a lot of ready-made names.</li>
