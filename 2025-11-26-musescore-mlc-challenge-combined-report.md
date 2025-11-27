---
date: 2025-11-26
author: Nick Blaskovich
repository: MuseScore
topic: "Million-Line Codebase Challenge - Combined Completion Report"
tags: [mlc-challenge, musescore, capo-chord-styling, lyrics-spellcheck, open-source, contribution]
status: complete
pull_requests:
  - url: "https://github.com/musescore/MuseScore/pull/31219"
    feature: "Capo Chord Styling"
    issue: "#18691"
  - url: "https://github.com/musescore/MuseScore/pull/31220"
    feature: "Lyrics Spellcheck"
    issue: null
---

# Million-Line Codebase Challenge - Combined Completion Report

**Date**: 2025-11-26
**Author**: Nick Blaskovich
**Repository**: MuseScore (https://github.com/musescore/MuseScore)

---

## Executive Summary

This report documents the completion of the Million-Line Codebase (MLC) Challenge for the MuseScore project. Two substantial end-to-end features were implemented:

1. **Capo Chord Styling** (PR [#31219](https://github.com/musescore/MuseScore/pull/31219)) - Enables musicians to display capo chord symbols in inline or stacked formats with extensive customization options. Resolves GitHub issue [#18691](https://github.com/musescore/MuseScore/issues/18691).

2. **Lyrics Spellcheck** (PR [#31220](https://github.com/musescore/MuseScore/pull/31220)) - Provides integrated spell checking for lyrics with a dialog interface for reviewing and navigating to spelling issues.

Both features are complete end-to-end implementations spanning data models, backend logic, rendering/service layers, and user interface components. All CI checks pass on both pull requests, and the CLA has been signed.

---

## Table of Contents

1. [Repository Qualification](#1-repository-qualification)
2. [Contribution Guidelines Compliance](#2-contribution-guidelines-compliance)
3. [Feature 1: Capo Chord Styling](#3-feature-1-capo-chord-styling)
4. [Feature 2: Lyrics Spellcheck](#4-feature-2-lyrics-spellcheck)
5. [Code Style Compliance](#5-code-style-compliance)
6. [Test Coverage](#6-test-coverage)
7. [Technical Documentation](#7-technical-documentation)
8. [Evaluation Criteria Mapping](#8-evaluation-criteria-mapping)
9. [Lessons Learned](#9-lessons-learned)
10. [Conclusion](#10-conclusion)

---

## 1. Repository Qualification

### 1.1 Repository Selection: MuseScore

MuseScore is a free, open-source music notation application used by millions of musicians worldwide. It provides professional-grade music engraving capabilities comparable to commercial software like Finale and Sibelius.

**Why MuseScore?**
- Active, well-maintained open-source project
- Clear contribution guidelines and established patterns
- Complex multi-layer architecture (engraving engine, UI, rendering)
- Real user impact - features benefit the global music community
- Exceeds 1 million lines of code

### 1.2 Lines of Code Verification

| Metric | Value | Source |
|--------|-------|--------|
| **Total Lines** | 8,191,486 code lines (8,958,270 total) | Open Hub |
| **C++ Application Code** | 770,556 lines | Open Hub |
| **QML UI Code** | 60,945 lines | Open Hub |
| **Combined Application Code** | ~831,500 lines | Calculated |

**Source**: [Open Hub MuseScore Project](https://openhub.net/p/musescore)

The repository significantly exceeds the 1 million line threshold. Open Hub reports "TypeScript" files which are actually Qt Linguist translation files (`.ts` XML files) accounting for 4.1M lines, plus XML resources totaling 2M lines.

### 1.3 Language Breakdown

| Language | Lines | Percentage |
|----------|-------|------------|
| **Qt Translation Files (.ts)** | **4,103,179** | **45.8%** |
| XML | 2,008,335 | 22.5% |
| C | 886,340 | 12.0% |
| **C++** | **770,556** | **8.6%** |
| Autoconf | 133,799 | 1.5% |
| HTML | 71,928 | 0.8% |
| Shell Script | 70,772 | 0.8% |
| **QML** | **60,945** | **0.7%** |

### 1.4 Active Maintenance

| Metric | Value |
|--------|-------|
| **Total Commits in 2025** | **6,289** |
| **Unique Contributors in 2025** | 107 |
| **Most Active Contributor** | Casper Jeukendrup (1,316 commits) |

**Top 5 Contributors in 2025:**
1. Casper Jeukendrup - 1,316 commits
2. Michele Spagnolo - 982 commits
3. James Mizen - 898 commits
4. Roman Pudashkin - 350 commits
5. rettinghaus - 312 commits

### 1.5 Contribution Guidelines

MuseScore has clear, well-documented contribution guidelines:
- [Contributing Wiki](https://github.com/musescore/MuseScore/wiki/Contributing)
- [Code Guidelines](https://github.com/musescore/muse_framework/wiki/CodeGuidelines)
- [Coding Rules](https://musescore.org/en/handbook/developers-handbook/finding-your-way-around/musescore-coding-rules)
- Contributor License Agreement (CLA) requirement
- CI-enforced code style checks

---

## 2. Contribution Guidelines Compliance

### 2.1 Requirements Checklist

| Requirement | Status | Evidence |
|-------------|--------|----------|
| **CLA Signed** | ✅ SIGNED | Completed at [musescore.org/cla](https://musescore.org/en/cla) |
| **Coding Rules** | ✅ COMPLIANT | Both PRs pass CI codestyle checks |
| **Unit Tests** | ✅ CREATED | 4 tests for capo, 8 tests for spellcheck |
| **Code Compiles** | ✅ YES | Both features build successfully |
| **Descriptive Titles** | ✅ YES | Clear PR titles describing features |
| **Commit Messages** | ✅ DESCRIPTIVE | All commits use imperative mood |
| **Issue Reference** | ✅ YES | Capo PR references #18691 |

### 2.2 PR Template Compliance

Both PRs follow the MuseScore PR template:

```markdown
- [x] I signed the CLA
- [x] The title of the PR describes the problem it addresses
- [x] Each commit's message describes its purpose and effects
- [x] If changes are extensive, there is a sequence of easily reviewable commits
- [x] The code in the PR follows the coding rules
- [x] There are no unnecessary changes
- [x] The code compiles and runs on my machine
- [x] I created a unit test to verify the changes I made
```

### 2.3 Code Style Tools

**Tools Used:**
- **Uncrustify**: MuseScore's code formatter with config at `tools/codestyle/uncrustify_musescore.cfg`
- **Pre-commit hook**: Available for automatic formatting
- **CI codestyle check**: Automated verification on PR submission

**Style Rules Followed:**
- 4-space indentation (no tabs)
- Consistent brace placement
- Proper namespace formatting
- SPDX license headers on all new files
- camelCase for variables/functions
- PascalCase for classes

---

## 3. Feature 1: Capo Chord Styling

### 3.1 Overview

**Pull Request**: [#31219](https://github.com/musescore/MuseScore/pull/31219)
**GitHub Issue**: [#18691](https://github.com/musescore/MuseScore/issues/18691)
**Branch**: `24413-capoed-chord-styling`

The Capo Chord Styling feature enables guitarists and arrangers to display chord symbols with capo transposition in two modes:

1. **INLINE Mode**: `Em(Dm)` - Traditional single-line display
2. **STACKED Mode**: Capo chord above, base chord below with customizable styling

### 3.2 Problem Solved

**Before:** Guitarists using capo had limited display options - only inline `Em(Dm)` format which is:
- Visually dense and hard to read
- Inconsistent with lead sheet conventions
- Required workarounds (extra staves, manual text)

**After:** Musicians can choose:
- **Stacked display** matching professional lead sheet conventions
- **Customizable styling** - different fonts for capo vs base chords
- **Automatic "Capo N:" labels** for clear indication
- **Configurable spacing** for optimal readability

### 3.3 Target Users

1. **Guitarists using capo on stage** - Quick visual reference
2. **Arrangers preparing lead sheets** - Professional appearance
3. **Music educators** - Clear teaching materials
4. **Worship musicians** - Common capo usage scenarios

### 3.4 Real-World Use Case

A guitarist playing in a worship band needs to see:
```
Capo 5:    (G)
            C     Am    F
```
Instead of the dense: `C(G) Am(Em) F(C)`

### 3.5 System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     USER INTERFACE                          │
│  ChordSymbolsPage.qml - 143 lines added                     │
│  chordsymbolspagemodel.cpp/h - 91 lines added               │
├─────────────────────────────────────────────────────────────┤
│                     RENDERING ENGINE                        │
│  harmonylayout.cpp - 432 lines modified                     │
│  systemlayout.cpp - 32 lines added                          │
├─────────────────────────────────────────────────────────────┤
│                     DATA MODEL                              │
│  styledef.h/cpp - 14 lines added                            │
│  types.h - 5 lines added                                    │
├─────────────────────────────────────────────────────────────┤
│                     TEST SUITE                              │
│  chordsymbol_tests.cpp - 97 lines added                     │
└─────────────────────────────────────────────────────────────┘
```

### 3.6 New Data Structures

#### 3.6.1 Display Mode Enum (types.h)
```cpp
enum class CapoChordDisplayMode : unsigned char {
    INLINE,     // Current behavior: Em(Dm)
    STACKED     // Capo chord above base chord
};
```

#### 3.6.2 Style Properties (styledef.h/cpp)
7 new style properties added:
- `capoChordDisplayMode` - INLINE/STACKED selection
- `capoChordParenthesized` - Show parentheses in stacked mode
- `capoChordTextStyle` - TextStyleType for capo chord (HARMONY_B default)
- `capoChordStackedSpacing` - Vertical gap (0.8 spatium default)
- `capoLabelVisible` - Show "Capo N:" label
- `capoLabelFormat` - Format string ("Capo %1:")
- `capoLabelTextStyle` - TextStyleType for label

### 3.7 Backend Logic

#### Key Functions Added (harmonylayout.cpp):
1. `buildFontFromTextStyle()` - Constructs Font from TextStyleType enum
2. `layoutCapoLabel()` - Positions "Capo N:" label left of first harmony
3. `calculateCapoTpc()` - Transposes chord roots for capo position
4. Modified `renderSingleHarmony()` - Handles both display modes

#### Stacked Mode Algorithm:
1. Render base chord to measure width
2. Build capo font from configured text style
3. Calculate vertical offset: `lineHeight = capHeight + gap`
4. Position capo chord at `-lineHeight` above base chord
5. Center capo chord horizontally over base chord
6. Pre-compensate for parenthesis shift if enabled

### 3.8 User Interface Components

**New UI Controls in Format → Style → Chord Symbols → Capo:**
1. Display mode radio buttons (Concert/Both/Transposed)
2. Capo fret position spinner (0-11)
3. Display mode dropdown (Inline/Stacked)
4. Capo chord text style selector
5. Parenthesized checkbox
6. Stacked spacing control
7. Label visibility checkbox
8. Label format text field
9. Label text style selector

### 3.9 Data Flow Diagram

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│   User UI    │────▶│  MStyle      │────▶│  Renderer    │
│  (QML/C++)   │     │  (Sid props) │     │  (Layout)    │
└──────────────┘     └──────────────┘     └──────────────┘
       │                    │                    │
       ▼                    ▼                    ▼
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ StyleItem    │     │ PropertyValue│     │ TextSegment  │
│ Q_PROPERTY   │     │ (int/bool/sp)│     │ RenderItems  │
└──────────────┘     └──────────────┘     └──────────────┘
```

### 3.10 Rendering Pipeline

```
HarmonyLayout::renderSingleHarmony()
           │
           ├──▶ displayCapo == CONCERT? ──▶ render base chord only
           │
           ├──▶ displayCapo == TRANSPOSED? ──▶ render capo chord only
           │
           └──▶ displayCapo == BOTH?
                      │
                      ├──▶ displayMode == INLINE?
                      │         │
                      │         └──▶ base + "(" + capo + ")"
                      │
                      └──▶ displayMode == STACKED?
                                │
                                ├──▶ Calculate lineHeight (capHeight + gap)
                                ├──▶ Build capo font from TextStyleType
                                ├──▶ Render base chord at y=0
                                ├──▶ Render capo chord at y=-lineHeight
                                ├──▶ Center capo horizontally
                                └──▶ Add parens if enabled
```

### 3.11 Technical Challenges Overcome

1. **Parenthesis Centering Bug**: `layoutModifierParentheses()` shifts items right. Solution: Measure shift amount, pre-compensate by starting rendering further left.

2. **Vertical Positioning**: Initially both chords shifted down. Solution: Keep base at y=0, position capo at negative offset.

3. **Font Building**: No existing function to build Font from TextStyleType. Solution: Created `buildFontFromTextStyle()` helper using Sid property lookups.

4. **Label Positioning**: Multiple call sites for harmony layout. Solution: Add label in `layoutCapoLabel()` called after harmony layout completes.

### 3.12 Design Decisions

| Decision | Rationale | Alternatives Considered |
|----------|-----------|------------------------|
| **Style-level only (no per-chord)** | Keeps implementation simple, matches existing style patterns | Per-chord overrides would require PropertyFlags changes |
| **HARMONY_B as default capo style** | Uses existing italic style, visually distinguishes capo chord | New dedicated text style, same as base chord |
| **0.8sp default spacing** | Visual testing showed good balance between compact and readable | 0.5sp (too tight), 1.0sp (too spread) |
| **Label as TextSegment** | Integrates with existing render pipeline, avoids element lifecycle complexity | Separate generated StaffText element |
| **Pre-compensate paren shift** | Ensures chord stays centered regardless of paren state | Post-adjustment, accept off-center |

### 3.13 Commit Structure

The 11 development commits were squashed into 4 logical commits for the PR:

| Commit | Hash | Description | Content |
|--------|------|-------------|---------|
| **1** | `3205dbe545` | Add CapoChordDisplayMode enum and style properties | Data model: types.h, styledef.h/cpp |
| **2** | `41821e67b0` | Implement stacked capo chord rendering and label display | Rendering: harmonylayout.cpp/h, systemlayout.cpp |
| **3** | `2821458158` | Add UI controls for capo chord styling options | UI: ChordSymbolsPage.qml, chordsymbolspagemodel.cpp/h |
| **4** | `ae533b6578` | Add unit tests for capo chord style properties | Tests: chordsymbol_tests.cpp |

**First commit message includes**: `Resolves: #18691`

### 3.14 Implementation Metrics

| Metric | Value |
|--------|-------|
| **Final Commits** | 4 (squashed from 11 development commits) |
| **Files Changed** | 10 |
| **Lines Added** | 774 |
| **Lines Removed** | 41 |
| **Net Lines** | +733 |
| **New Tests** | 4 test functions |
| **New Style Properties** | 7 |

### 3.15 Feature Completeness

| Feature | Status |
|---------|--------|
| INLINE display mode | ✅ Complete |
| STACKED display mode | ✅ Complete |
| Parentheses toggle | ✅ Complete |
| Capo chord text style | ✅ Complete |
| Configurable spacing | ✅ Complete |
| Capo label rendering | ✅ Complete |
| Label format customization | ✅ Complete |
| UI controls | ✅ Complete |
| Serialization | ✅ Complete |
| Backwards compatibility | ✅ Complete |
| Unit tests | ✅ Complete |

---

## 4. Feature 2: Lyrics Spellcheck

### 4.1 Overview

**Pull Request**: [#31220](https://github.com/musescore/MuseScore/pull/31220)
**Branch**: `spell-check`

The Lyrics Spellcheck feature provides integrated spell checking for lyrics within MuseScore, allowing users to identify and navigate to misspelled words without leaving the application.

### 4.2 Problem Solved

**Before:** Lyrics entry in MuseScore was error-prone with no built-in spell checking. The recommended workaround was:
- Tools → Copy lyrics to clipboard
- Paste into a word processor
- Spellcheck externally
- Manually fix typos back in MuseScore

This workflow is slow and brittle, especially on large scores or multi-verse choral works.

**After:** Musicians can:
- Spellcheck all lyrics directly within MuseScore
- See a list of misspelled words with occurrence counts
- Navigate directly to each issue in the score
- Rescan after making corrections

### 4.3 User Stories Addressed

1. **Choir director proofreading lyrics**: Can now spellcheck all lyrics from inside MuseScore
2. **Arranger with multi-verse lyrics**: Gets a list of suspicious words with locations (measure, staff, verse)
3. **User with mismatched dictionaries**: System auto-detects language from locale with fallback

### 4.4 Feature Benefits

| Benefit | Description |
|---------|-------------|
| Time saving | No more copy-paste to external programs |
| Context preservation | Navigate directly to lyric in score |
| Multi-verse support | Checks all staves and verses simultaneously |
| Location tracking | Shows exact measure, staff, and verse for each issue |
| Rescan capability | Can re-run after fixing issues |
| Non-destructive | Read-only; user makes corrections manually |

### 4.5 System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         USER INTERFACE                          │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ LyricsSpellingIssuesDialog.qml (216 lines)               │  │
│  │ - StyledDialogView with list of issues                   │  │
│  │ - "Go to", "Rescan", "Close" buttons                     │  │
│  │ - Double-click navigation                                │  │
│  └────────────────────────┬─────────────────────────────────┘  │
│                           │                                     │
│  ┌────────────────────────▼─────────────────────────────────┐  │
│  │ LyricsSpellingIssuesModel (158 lines)                    │  │
│  │ - QAbstractListModel for QML binding                     │  │
│  │ - Roles: word, count, location                           │  │
│  │ - goToIssue() navigation method                          │  │
│  └────────────────────────┬─────────────────────────────────┘  │
└───────────────────────────┼─────────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────────┐
│                      BUSINESS LOGIC                              │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ LyricsSpellCheckService (258 lines)                      │  │
│  │ - Collects lyrics via Score::lyrics()                    │  │
│  │ - Reconstructs words from syllables using LyricsSyllabic │  │
│  │ - Maps words to Lyrics* elements for navigation          │  │
│  │ - Formats location strings (measure, staff, verse)       │  │
│  └────────────────────────┬─────────────────────────────────┘  │
│                           │                                     │
│  ┌────────────────────────▼─────────────────────────────────┐  │
│  │ ILyricsSpellCheckService Interface (70 lines)            │  │
│  │ - checkLyrics(INotationPtr) → LyricsSpellCheckResult     │  │
│  │ - LyricsSpellCheckIssue struct with word, count, location│  │
│  └──────────────────────────────────────────────────────────┘  │
└───────────────────────────┼─────────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────────┐
│                    SPELLCHECK BACKEND                            │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ HunspellSpellChecker (216 lines)                         │  │
│  │ - Hunspell library integration via conditional compile   │  │
│  │ - Platform-specific dictionary discovery                 │  │
│  │ - Language selection from system locale                  │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ ISpellChecker Interface (48 lines)                       │  │
│  │ - isCorrect(word), suggestions(word)                     │  │
│  │ - availableLanguages(), setLanguage()                    │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ StubSpellChecker (40 lines)                              │  │
│  │ - Fallback when Hunspell unavailable                     │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────────┐
│                    BUILD SYSTEM                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ CMakeLists.txt (root) - MUE_COMPILE_USE_SYSTEM_HUNSPELL  │  │
│  │ SetupHunspell.cmake - Library detection via pkg-config   │  │
│  │ notation/CMakeLists.txt - Conditional compilation        │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────────┐
│                    INTEGRATION POINTS                            │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ NotationActionController - checkLyricsSpelling() handler │  │
│  │ NotationUiActions - "check-lyrics-spelling" action       │  │
│  │ AppMenuModel - Tools menu entry                          │  │
│  │ NotationModule - IoC registration, QML type registration │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### 4.6 New Files Created

| Category | Files | Lines |
|----------|-------|-------|
| Interfaces | `ispellchecker.h`, `ilyricsspellcheckservice.h` | 118 |
| Backend | `hunspellspellchecker.h/.cpp`, `stubspellchecker.h` | 318 |
| Service | `lyricsspellcheckservice.h/.cpp` | 308 |
| UI Model | `lyricsspellingissuesmodel.h/.cpp` | 240 |
| QML Dialog | `LyricsSpellingIssuesDialog.qml` | 216 |
| Build System | `SetupHunspell.cmake`, CMakeLists changes | ~78 |
| Integration | Action controller, UI actions, menu model changes | ~53 |
| Tests | `lyricsspellcheck_tests.cpp`, `mocks/spellcheckermock.h` | ~200 |

### 4.7 Data Structures

#### LyricsSpellCheckIssue (ilyricsspellcheckservice.h):
```cpp
struct LyricsSpellCheckIssue {
    QString word;                              // The misspelled word
    int occurrenceCount = 0;                   // How many times it appears
    QString firstLocation;                     // Human-readable: "m. 12, Soprano, Verse 1"
    mu::engraving::Lyrics* firstLyrics = nullptr;  // For navigation
    QList<mu::engraving::Lyrics*> allOccurrences;  // All instances
};
```

#### LyricsSpellCheckResult (ilyricsspellcheckservice.h):
```cpp
struct LyricsSpellCheckResult {
    bool spellCheckerAvailable = false;
    QString language;
    QString errorMessage;
    QList<LyricsSpellCheckIssue> issues;
    int totalLyricsChecked = 0;
    int totalWordsChecked = 0;
};
```

### 4.8 Component Diagram

```
┌────────────────────────────────────────────────────────────────────┐
│                          NOTATION MODULE                            │
├────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────┐     ┌──────────────────────┐                  │
│  │ NotationAction  │────►│ checkLyricsSpelling()│                  │
│  │   Controller    │     └──────────┬───────────┘                  │
│  └─────────────────┘                │                              │
│                                     ▼                              │
│  ┌─────────────────────────────────────────────────────────┐       │
│  │              ILyricsSpellCheckService                    │       │
│  │  ┌─────────────────────────────────────────────────┐    │       │
│  │  │          LyricsSpellCheckService                │    │       │
│  │  │  - checkLyrics(notation) → Result               │    │       │
│  │  │  - Injects: ISpellChecker                       │    │       │
│  │  └───────────────────┬─────────────────────────────┘    │       │
│  └──────────────────────┼──────────────────────────────────┘       │
│                         │                                          │
│                         ▼                                          │
│  ┌──────────────────────────────────────────────────────────┐      │
│  │                    ISpellChecker                          │      │
│  │  ┌────────────────────┐    ┌─────────────────────┐       │      │
│  │  │HunspellSpellChecker│    │  StubSpellChecker   │       │      │
│  │  │  (when available)  │    │  (fallback)         │       │      │
│  │  └────────────────────┘    └─────────────────────┘       │      │
│  └──────────────────────────────────────────────────────────┘      │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────┐      │
│  │            LyricsSpellingIssuesModel (QML)               │      │
│  │  - Binds to LyricsSpellingIssuesDialog.qml               │      │
│  │  - goToIssue() → interaction->select() + showItem()      │      │
│  └──────────────────────────────────────────────────────────┘      │
│                                                                     │
└────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌────────────────────────────────────────────────────────────────────┐
│                         ENGRAVING MODULE                            │
├────────────────────────────────────────────────────────────────────┤
│  Score::lyrics() → std::vector<Lyrics*>                            │
│  Lyrics::plainText(), syllabic(), verse(), measure(), staffIdx()   │
└────────────────────────────────────────────────────────────────────┘
```

### 4.9 Data Flow Diagram

```
User clicks "Tools > Check lyrics spelling..."
                    │
                    ▼
        NotationActionController
        ::checkLyricsSpelling()
                    │
                    ▼
    ┌───────────────────────────────┐
    │ Check spellChecker available? │
    └───────────────┬───────────────┘
                    │
         ┌──────────┴──────────┐
         │                     │
    [NO] ▼                     ▼ [YES]
  Show warning          LyricsSpellCheckService
    dialog                 ::checkLyrics()
                              │
                    ┌─────────┴─────────┐
                    │                   │
                    ▼                   ▼
            Score::lyrics()      For each word:
            (get all lyrics)     spellChecker->isCorrect()
                    │                   │
                    │                   ▼
                    │           Build LyricsSpellCheckIssue
                    │           with word, count, location
                    │                   │
                    └────────┬──────────┘
                             │
                             ▼
               LyricsSpellCheckResult
               {issues[], language, stats}
                             │
                             ▼
            interactive->open("lyricsspellingissues")
                             │
                             ▼
           LyricsSpellingIssuesDialog.qml
           (displays issues in list view)
                             │
              ┌──────────────┴──────────────┐
              │                             │
         [Go to]                       [Rescan]
              │                             │
              ▼                             ▼
    issuesModel.goToIssue()      issuesModel.rescan()
              │                             │
              ▼                             │
    interaction->select(lyrics)             │
    interaction->showItem(lyrics)           │
              │                             │
              ▼                             ▼
    Score view scrolls to         Re-run checkLyrics()
    selected lyric element        Update model
```

### 4.10 Integration with Existing Systems

1. **Lyrics Model Integration**: Uses existing `Score::lyrics()` and `Lyrics` class (`engraving/dom/lyrics.h`)
2. **Navigation**: Leverages `INotationInteraction::select()` and `showItem()` patterns
3. **IoC Container**: Registers via `modularity::ioc()->registerExport<>()` pattern
4. **QML Dialog System**: Uses `StyledDialogView` base and `qsTrc()` for translations

### 4.11 Existing Patterns Followed

#### Action Registration Pattern
Matches `copy-lyrics-to-clipboard` pattern exactly:

**UiAction** (`notationuiactions.cpp:520-523`):
```cpp
UiAction("check-lyrics-spelling",
         mu::context::UiCtxProjectOpened,
         mu::context::CTX_NOTATION_OPENED,
         TranslatableString("action", "Check lyrics &spelling..."),
         TranslatableString("action", "Check lyrics spelling")
         ),
```

**Handler** (`notationactioncontroller.cpp`):
```cpp
registerAction("check-lyrics-spelling", [this]() { checkLyricsSpelling(); });
```

**Menu** (`appmenumodel.cpp:422`):
```cpp
makeMenuItem("check-lyrics-spelling"),
```

#### QML Dialog Pattern
Follows `PartsDialog.qml` structure:
- `StyledDialogView` base component
- Model instantiation with `Component.onCompleted` → `model.load()`
- `ColumnLayout` with margins
- Button row at bottom with Close button

#### Module Export Pattern
Follows existing IoC registration in `notationmodule.cpp`:
```cpp
ioc()->registerExport<ISpellChecker>(moduleName(), new HunspellSpellChecker());
ioc()->registerExport<ILyricsSpellCheckService>(moduleName(), new LyricsSpellCheckService());
```

#### CMake Pattern
Follows `SetupFLAC.cmake` / `SetupFreeType.cmake` pattern:
- pkg-config first, then find_library fallback
- Platform-specific search paths
- Conditional compilation via `#ifdef MUE_BUILD_NOTATION_HUNSPELL`

### 4.12 Design Decisions

| Decision | Rationale | Alternative Considered |
|----------|-----------|------------------------|
| Use system Hunspell | Avoid bundling large dictionary files; leverage existing user installations | Bundle Hunspell + dictionaries (rejected: +100MB distribution size) |
| QML dialog not Qt Widgets | Modern MuseScore 4 uses QML for new UI; matches existing patterns | Qt Widgets (rejected: older pattern being phased out) |
| Read-only v1 | Reduce scope; let users learn the feature; avoid complex text editing | Auto-replace (rejected: too complex for v1) |
| Single language | Simplify MVP; multi-language adds complexity | Per-verse language detection (deferred to v2) |
| Syllable reconstruction | More accurate spellcheck for hyphenated lyrics | Check each syllable separately (rejected: too many false positives) |
| StubSpellChecker fallback | Graceful degradation when Hunspell unavailable | Disable menu item (rejected: confusing UX) |
| `alwaysOnTop: true` | Let users edit score while dialog stays visible | Modal dialog (initially tried; user requested non-modal) |

### 4.13 Commit Structure

The feature was organized into 4 logical, reviewable commits:

| Commit | Hash | Description |
|--------|------|-------------|
| **1** | `4f03d21234` | Add Hunspell spellchecker infrastructure |
| **2** | `ed30d9a41c` | Add lyrics spellcheck service |
| **3** | `ed3fa6e103` | Add lyrics spelling issues dialog and menu integration |
| **4** | `e1348aaebf` | Add unit tests for lyrics spellcheck service |

### 4.14 Implementation Metrics

| Category | Files | Lines |
|----------|-------|-------|
| Total new files | 21 | ~1,500 |
| Interfaces | 2 | 118 |
| Backend implementation | 3 | 318 |
| Service layer | 2 | 308 |
| UI (Model + QML) | 3 | 456 |
| Tests | 2 | ~200 |
| Build system | 1 + modifications | ~78 |

### 4.15 Feature Completeness

| Feature | Status |
|---------|--------|
| Hunspell integration | ✅ Complete |
| Platform-specific dictionary discovery | ✅ Complete |
| Lyrics collection from score | ✅ Complete |
| Syllable reconstruction | ✅ Complete |
| Spelling issue detection | ✅ Complete |
| Location formatting | ✅ Complete |
| QML dialog UI | ✅ Complete |
| Navigation to issues | ✅ Complete |
| Rescan capability | ✅ Complete |
| Graceful fallback | ✅ Complete |
| Unit tests | ✅ Complete |

---

## 5. Code Style Compliance

### 5.1 Naming Conventions

| Convention | Capo Feature | Lyrics Spellcheck |
|------------|--------------|-------------------|
| camelCase variables/functions | `capoChordDisplayMode`, `buildFontFromTextStyle()` | `discoverDictionaries()`, `isAvailable()`, `m_hunspell` |
| PascalCase classes | `CapoChordDisplayMode` | `HunspellSpellChecker`, `LyricsSpellCheckService`, `LyricsSpellingIssuesModel` |
| Sid:: prefix for styles | `Sid::capoChordDisplayMode` | N/A |

### 5.2 Code Style Examples

**License Header** (all new files):
```cpp
/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2024 MuseScore Limited
 * ...
 */
```

**Namespace Usage**:
```cpp
namespace mu::notation {
// Code here
}
```

**QML Style**:
- Uses `qsTrc()` for translatable strings
- Follows `StyledDialogView` pattern
- Uses MuseScore UI components (`StyledTextLabel`, `FlatButton`, `StyledListView`)

### 5.3 CI Codestyle Fixes Applied

Both PRs required codestyle fixes to pass CI:

**Capo Feature:**
- Constructor initializer list formatting preserved from original
- Macro formatting preserved for `BEGIN_QT_REGISTERED_ENUM`/`END_QT_REGISTERED_ENUM`

**Lyrics Spellcheck:**
- Namespace formatting: Removed extra blank lines after `namespace mu::notation {`
- Lambda indentation: Aligned lambda body with 4-space indent from `std::sort`
- Method chain alignment: Aligned `.arg()` calls with opening `QString`
- Constructor formatting: Removed extra blank line before closing brace

---

## 6. Test Coverage

### 6.1 Capo Chord Styling Tests

**File**: `src/engraving/tests/chordsymbol_tests.cpp`

| Test Name | Purpose | Coverage |
|-----------|---------|----------|
| `capoDisplayModeRoundTrip` | Verifies save/load of display mode and parens | Serialization |
| `capoDisplayModeDefaults` | Verifies backwards compatibility defaults | Migration |
| `capoEnhancedStylesRoundTrip` | Verifies all 5 enhanced properties persist | Serialization |
| `capoEnhancedStylesDefaults` | Verifies enhanced property defaults | Migration |

**Coverage Analysis:**

| Component | Test Coverage | Notes |
|-----------|---------------|-------|
| Style Properties | 100% | All 7 properties have round-trip tests |
| Default Values | 100% | All defaults verified in tests |
| Rendering Logic | Manual | Visual verification required |
| UI Controls | Manual | QML testing not automated |

### 6.2 Lyrics Spellcheck Tests

**Files**:
- `src/notation/tests/lyricsspellcheck_tests.cpp`
- `src/notation/tests/mocks/spellcheckermock.h`

**Mock Class**:
```cpp
class SpellCheckerMock : public ISpellChecker
{
public:
    MOCK_METHOD(bool, isAvailable, (), (const, override));
    MOCK_METHOD(QString, language, (), (const, override));
    MOCK_METHOD(QStringList, availableLanguages, (), (const, override));
    MOCK_METHOD(bool, setLanguage, (const QString&), (override));
    MOCK_METHOD(bool, isCorrect, (const QString&), (const, override));
    MOCK_METHOD(QStringList, suggestions, (const QString&), (const, override));
};
```

**Test Cases**:

| Test Name | Description |
|-----------|-------------|
| `isAvailable_WhenSpellCheckerAvailable_ReturnsTrue` | Service reports available when spellchecker is available |
| `isAvailable_WhenSpellCheckerUnavailable_ReturnsFalse` | Service reports unavailable when spellchecker is unavailable |
| `availableLanguages_ReturnsSpellCheckerLanguages` | Language list passed through correctly |
| `currentLanguage_ReturnsSpellCheckerLanguage` | Current language delegation works |
| `setLanguage_WhenSuccessful_ReturnsTrue` | Language change success propagated |
| `setLanguage_WhenFailed_ReturnsFalse` | Language change failure propagated |
| `checkLyrics_WhenSpellCheckerUnavailable_ReturnsError` | Proper error when spellchecker unavailable |
| `checkLyrics_WhenNotationNull_ReturnsError` | Proper error when notation is null |

### 6.3 Testing Patterns Followed

| Pattern | Both Features |
|---------|---------------|
| Framework | Google Test (gtest) + Google Mock (gmock) |
| Test file naming | `*_tests.cpp` |
| Test class naming | `Module_FeatureTests : public ::testing::Test` |
| Mock class location | `src/*/tests/mocks/` directory |
| IoC injection | Register mock in SetUp, unregister in TearDown |
| Round-trip tests | Load → Modify → Save → Reload → Verify |
| Default value tests | Verify backwards compatibility |
| Memory cleanup | `delete score;` at end of tests |

### 6.4 Manual Testing Completed

Both features underwent comprehensive manual testing:

**Capo Chord Styling:**
- Toggle between INLINE and STACKED modes
- Toggle parentheses on/off in STACKED mode
- Adjust spacing from 0.0 to 2.0sp
- Toggle "Show capo position label"
- Change label format
- Save, close, reopen score for persistence

**Lyrics Spellcheck:**
- Basic functionality tests
- Multi-verse/multi-staff tests
- Edge case tests (empty lyrics, punctuation, numbers)
- Dialog behavior tests
- Navigation to issues

---

## 7. Technical Documentation

### 7.1 Documentation Created

| Document | Location | Purpose |
|----------|----------|---------|
| **Capo Feature Spec** | `ai-notes/capo-chords/spec.md` | Requirements and design |
| **Capo Research: Initial** | `thoughts/shared/research/2025-11-24-capo-chord-display-implementation.md` | Codebase exploration |
| **Capo Research: Enhanced** | `thoughts/shared/research/2025-11-25-capo-chord-enhanced-styling.md` | Text style system research |
| **Capo Plan: Stacked Display** | `thoughts/shared/plans/2025-11-24-capo-chord-stacked-display.md` | Implementation plan |
| **Capo Plan: Enhanced Styling** | `thoughts/shared/plans/2025-11-25-capo-chord-enhanced-styling.md` | Enhanced features plan |
| **Capo Completion Report** | `thoughts/shared/research/2025-11-26-capo-chord-mlc-completion-report.md` | Feature documentation |
| **Lyrics Architecture** | `thoughts/shared/research/2025-11-25-lyrics-spellcheck-architecture.md` | Initial codebase research |
| **Lyrics Implementation Plan** | `thoughts/shared/plans/2025-11-25-lyrics-spellcheck.md` | Implementation plan |
| **Lyrics MLC Compliance** | `thoughts/shared/research/2025-11-26-lyrics-spellcheck-mlc-compliance.md` | Feature documentation |
| **Original Spec** | `sc-issue.md` | Original feature specification |
| **This Combined Report** | `thoughts/shared/research/2025-11-26-musescore-mlc-challenge-combined-report.md` | Combined completion documentation |

### 7.2 Setup Instructions

#### Prerequisites

**For Lyrics Spellcheck - Hunspell Installation:**

**macOS**:
```bash
brew install hunspell
# Dictionaries typically in /opt/homebrew/share/hunspell/
# If not present, install specific language:
brew install hunspell-en_us
```

**Linux (Ubuntu/Debian)**:
```bash
sudo apt install hunspell libhunspell-dev hunspell-en-us
```

**Windows**:
Install hunspell and place dictionaries in `%APPDATA%/hunspell/`

#### Building MuseScore

```bash
# Clone the repository
git clone https://github.com/musescore/MuseScore.git
cd MuseScore

# For Capo Feature:
git fetch origin 24413-capoed-chord-styling
git checkout 24413-capoed-chord-styling

# For Lyrics Spellcheck:
git fetch origin spell-check
git checkout spell-check

# Build (Release mode)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMUE_COMPILE_USE_SYSTEM_HUNSPELL=ON
cmake --build build -j$(nproc)
```

#### Running Tests

```bash
# Capo chord tests
ctest --test-dir build -R chordsymbol
ctest --test-dir build -R capo

# Lyrics spellcheck tests
ctest --test-dir build -R lyricsspellcheck
```

### 7.3 Manual Testing Steps

#### Capo Chord Styling:
1. Launch MuseScore: `./build/install/bin/mscore4portable`
2. Create new score with guitar staff
3. Add chord symbols: C, Am, F, G
4. Open **Format → Style → Chord Symbols → Capo**
5. Set capo position: 5
6. Select display mode: "Show chord symbols & transposed symbols"
7. Test INLINE vs STACKED modes
8. Toggle parentheses, adjust spacing
9. Enable/configure capo label
10. Verify persistence: Save, close, reopen score

#### Lyrics Spellcheck:
1. Launch MuseScore
2. Open a score with lyrics (or create one with intentional typos)
3. Go to **Tools > Check lyrics spelling...**
4. Verify dialog shows misspelled words
5. Click on an issue and press "Go to"
6. Verify score navigates to the correct lyric
7. Fix a typo, click "Rescan"
8. Verify the fixed word disappears from the list

---

## 8. Evaluation Criteria Mapping

### 8.1 Technical Depth (40%)

| Criterion | Capo Chord Styling | Lyrics Spellcheck |
|-----------|-------------------|-------------------|
| **Complexity** | Custom rendering algorithm for stacked chords, font building from style properties, parenthesis centering compensation | Hunspell library integration, syllable reconstruction algorithm, cross-module IoC integration |
| **Architecture** | Extends existing style system with 7 new properties, rendering pipeline modifications | New service layer, interface abstractions, conditional compilation |
| **Integration** | Integrates with MStyle, HarmonyLayout, QML style pages | Integrates with Score lyrics API, NotationInteraction, QML dialog system |
| **Layers Touched** | Data model, rendering engine, UI | Backend, service, UI model, QML, build system, menu system |

### 8.2 Completeness (30%)

| Criterion | Capo Chord Styling | Lyrics Spellcheck |
|-----------|-------------------|-------------------|
| **End-to-End** | ✅ Data model → Rendering → UI → Tests | ✅ Backend → Service → UI → Integration → Tests |
| **Tests** | ✅ 4 unit tests (round-trip, defaults) | ✅ 8 unit tests with mocks |
| **Documentation** | ✅ Specs, research, plans, completion report | ✅ Architecture, plans, compliance report |
| **Serialization** | ✅ Full save/load support | N/A (runtime only) |
| **Backwards Compatibility** | ✅ Verified via defaults tests | ✅ Graceful fallback when unavailable |

### 8.3 Communication (20%)

| Criterion | Evidence |
|-----------|----------|
| **PR Quality** | Both PRs follow template, have descriptive titles, reference issues where applicable |
| **Commit Messages** | Imperative mood, logical grouping, descriptive content |
| **Documentation** | Comprehensive research documents, architecture diagrams, setup instructions |
| **Code Comments** | Self-documenting code with descriptive variable names |

### 8.4 Learning (10%)

| Criterion | Evidence |
|-----------|----------|
| **Codebase Understanding** | Extensive research documented in thoughts/ directory |
| **Pattern Recognition** | Both features follow established MuseScore patterns exactly |
| **Challenges Overcome** | Documented technical challenges and solutions in decision logs |
| **Process Improvement** | See Section 9: Lessons Learned |

---

## 9. Lessons Learned

### 9.1 Contributing to Open Source

Through this project, I learned significantly about contributing to open source libraries. Key takeaways include:

- **Following coding practices**: Understanding and adhering to project-specific code style, naming conventions, and architectural patterns is essential for acceptance
- **Clean commit history**: The importance of squashing development commits into logical, reviewable units before PR submission
- **CI compliance**: Local tools may differ from CI configuration; always verify against actual CI checks

### 9.2 Approaching Brownfield Projects

The most valuable learning was developing a systematic approach to brownfield (existing) codebases:

1. **Research First**: Spend significant time understanding existing patterns before writing code
2. **Develop a Plan**: Create detailed implementation plans based on research findings
3. **Execute the Plan**: Follow the plan systematically, documenting deviations and challenges
4. **Iterate**: Be prepared to adjust plans based on discoveries during implementation

### 9.3 Confidence in Repository Navigation

This challenge built confidence in the ability to:

- Quickly understand a large, unfamiliar repository
- Identify the right patterns to follow for new features
- Make impactful contributions in a short timeframe
- Navigate complex multi-layer architectures

### 9.4 Technical Insights

**Rendering Systems**: The capo feature revealed the complexity of music notation rendering, particularly:
- Font management and text measurement
- Coordinate systems and positioning
- The interplay between style properties and visual output

**Integration Patterns**: The lyrics spellcheck feature demonstrated:
- Clean interface abstraction for external library integration
- Graceful degradation when dependencies are unavailable
- IoC container patterns for testability

---

## 10. Conclusion

### 10.1 Summary

This report documents the successful completion of the Million-Line Codebase Challenge with two substantial features for MuseScore:

| Aspect | Capo Chord Styling | Lyrics Spellcheck |
|--------|-------------------|-------------------|
| **PR** | [#31219](https://github.com/musescore/MuseScore/pull/31219) | [#31220](https://github.com/musescore/MuseScore/pull/31220) |
| **Issue** | [#18691](https://github.com/musescore/MuseScore/issues/18691) | N/A |
| **Lines Added** | ~733 | ~1,500 |
| **Tests** | 4 | 8 |
| **CI Status** | ✅ Passing | ✅ Passing |
| **CLA** | ✅ Signed | ✅ Signed |

### 10.2 Requirements Met

| MLC Requirement | Status | Evidence |
|-----------------|--------|----------|
| Repository >1M lines | ✅ | 8.2M+ lines (Open Hub verified) |
| Active maintenance | ✅ | 6,289 commits in 2025, 107 contributors |
| Clear contribution guidelines | ✅ | Wiki, CLA, CI checks |
| Two end-to-end features | ✅ | Both span multiple system layers |
| Real user value | ✅ | Solves documented user problems |
| Follow project patterns | ✅ | Linters, style, testing patterns |
| Pull requests submitted | ✅ | #31219, #31220 |
| Technical documentation | ✅ | Architecture diagrams, setup instructions, decision logs |
| Test coverage | ✅ | 12 total unit tests |

### 10.3 Feature Strengths

**Capo Chord Styling:**
- Resolves a real GitHub issue (#18691)
- Comprehensive customization options
- Full backwards compatibility
- Professional-grade lead sheet appearance

**Lyrics Spellcheck:**
- Solves documented user pain point
- Clean integration with existing architecture
- Graceful degradation when dependencies unavailable
- Well-documented development process

### 10.4 Pull Request Links

- **Capo Chord Styling**: https://github.com/musescore/MuseScore/pull/31219
- **Lyrics Spellcheck**: https://github.com/musescore/MuseScore/pull/31220

---

## Appendix A: Code References

### Capo Chord Styling
- `src/engraving/dom/types.h` - CapoChordDisplayMode enum
- `src/engraving/style/styledef.h` - Style property declarations
- `src/engraving/style/styledef.cpp` - Style property defaults
- `src/engraving/rendering/score/harmonylayout.cpp` - Rendering implementation
- `src/engraving/rendering/score/harmonylayout.h` - Function declarations
- `src/engraving/rendering/score/systemlayout.cpp` - Label integration
- `src/notation/view/styledialog/chordsymbolspagemodel.cpp` - UI model
- `src/notation/view/styledialog/chordsymbolspagemodel.h` - UI model header
- `src/notation/qml/MuseScore/NotationScene/internal/ChordSymbolsPage.qml` - QML UI
- `src/engraving/tests/chordsymbol_tests.cpp` - Unit tests

### Lyrics Spellcheck
- `src/notation/ispellchecker.h` - Spellchecker interface
- `src/notation/internal/hunspellspellchecker.cpp` - Hunspell implementation
- `src/notation/internal/hunspellspellchecker.h` - Implementation header
- `src/notation/internal/stubspellchecker.h` - Stub fallback
- `src/notation/ilyricsspellcheckservice.h` - Service interface
- `src/notation/internal/lyricsspellcheckservice.cpp` - Service implementation
- `src/notation/internal/lyricsspellcheckservice.h` - Service header
- `src/notation/view/lyricsspellingissuesmodel.cpp` - QML model
- `src/notation/view/lyricsspellingissuesmodel.h` - Model header
- `src/notation/qml/MuseScore/NotationScene/LyricsSpellingIssuesDialog.qml` - Dialog UI
- `src/notation/internal/notationactioncontroller.cpp` - Action handler
- `src/notation/internal/notationuiactions.cpp` - Action definition
- `src/appshell/view/appmenumodel.cpp` - Menu entry
- `src/notation/notationmodule.cpp` - Module registration
- `src/notation/cmake/SetupHunspell.cmake` - Build configuration
- `src/notation/tests/lyricsspellcheck_tests.cpp` - Unit tests
- `src/notation/tests/mocks/spellcheckermock.h` - Test mock

---

## Appendix B: External Sources

- [MuseScore GitHub Repository](https://github.com/musescore/MuseScore)
- [Open Hub MuseScore Project](https://openhub.net/p/musescore)
- [MuseScore Contributing Guidelines](https://github.com/musescore/MuseScore/wiki/Contributing)
- [MuseScore Code Guidelines](https://github.com/musescore/muse_framework/wiki/CodeGuidelines)
- [MuseScore Coding Rules](https://musescore.org/en/handbook/developers-handbook/finding-your-way-around/musescore-coding-rules)
- [MuseScore CodeStructure](https://github.com/musescore/MuseScore/wiki/CodeStructure)
