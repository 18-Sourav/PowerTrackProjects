# 🎵 MP3 Tag Reader and Editor (ID3v2)

A command-line application written in **C** that allows users to **view** and **edit** ID3v2 metadata tags stored in MP3 files. The tool supports both **ID3v2.3** and **ID3v2.4** tag formats, making it easy to manage song information directly from the terminal.

---

## 📌 Features

### View Metadata
Display the following ID3 tags from an MP3 file:

- 🎵 Title
- 🎤 Artist
- 💿 Album
- 📅 Year
- 🎼 Genre
- 🔢 Track Number

### Edit Metadata

Modify individual metadata fields without affecting the audio content.

Supported editable tags include:

- Title
- Artist
- Album
- Year
- Genre
- Track Number

### Validation

The application performs several checks before processing a file:

- Verifies `.mp3` file extension
- Checks for a valid ID3 header
- Supports ID3v2.3 and ID3v2.4 versions
- Handles invalid or corrupted metadata gracefully

---

# 🛠️ Technologies Used

- C Programming
- File Handling
- Binary File Operations
- Structures
- Command-Line Argument Parsing
- Bitwise Operations

---

# 📂 Project Structure

```
.
├── main.c          # Entry point
├── view.c          # Metadata viewing functions
├── edit.c          # Metadata editing functions
├── view.h
├── edit.h
├── types.h
├── Song.mp3      # Sample MP3 file (optional)
└── README.md
```

---

# ⚙️ Compilation

Compile all source files using GCC.

```bash
gcc main.c view.c edit.c -o mp3_tag_reader
```

---

# ▶️ Usage

## View MP3 Metadata

```bash
./mp3_tag_reader -v song.mp3
```

Example Output

```
-----------------------------
MP3 TAG INFORMATION
-----------------------------
Title        : Believer
Artist       : Imagine Dragons
Album        : Evolve
Year         : 2017
Genre        : Rock
Track Number : 01
```

---

## Edit Metadata

### Change Title

```bash
./mp3_tag_reader -e -t "New Title" song.mp3
```


# 📖 Command Summary

| Command | Description |
|----------|-------------|
| `-v` | View metadata |
| `-e` | Edit metadata |
| `-t` | Edit Title |
| `-a` | Edit Artist |
| `-A` | Edit Album |
| `-y` | Edit Year |
| `-g` | Edit Genre |
| `-n` | Edit Track Number |

---

# 🔍 How It Works

1. Opens the MP3 file in binary mode.
2. Reads the ID3 header.
3. Verifies the ID3 version.
4. Parses individual frames (TIT2, TPE1, TALB, TYER/TDRC, TCON, TRCK).
5. Displays metadata or updates the selected frame.
6. Writes the modified metadata back to the MP3 file while preserving the audio data.

---

# 🧪 Example

### View Metadata

```bash
$ ./mp3_tag_reader -v music.mp3
```

Output

```
Title        : Shape of You
Artist       : Ed Sheeran
Album        : Divide
Year         : 2017
Genre        : Pop
Track Number : 04
```

### Update Artist

```bash
$ ./mp3_tag_reader -e -a "John Doe" music.mp3
```

Output

```
Artist updated successfully.
```

---

# ⚠️ Limitations

- Supports only ID3v2.3 and ID3v2.4 tags.
- Does not edit audio content.
- Assumes standard ID3 frame structure.

---

# 🚀 Future Improvements

- Support for ID3v1 tags
- Batch editing multiple MP3 files
- Album artwork (APIC) support
- Lyrics (USLT) support
- UTF-16 and UTF-8 encoding improvements
- Interactive menu-driven interface

---

# 👨‍💻 Author

**Sourav Shukla**
