# Contributing to Edito

First off, thank you for considering contributing to **Edito**! 🎉  
Edito is a text editor built with Qt and C++, and every contribution—no matter how small—helps make it better.

This document will guide you through the entire process, from reporting a bug to getting your code merged.  
We want contributing to be a positive experience for everyone, so please also read our [Code of Conduct](CODE_OF_CONDUCT.md).

---

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Your First Code Contribution](#your-first-code-contribution)
- [Development Environment Setup](#development-environment-setup)
- [The Contribution Workflow](#the-contribution-workflow)
  - [1. Find or Create an Issue](#1-find-or-create-an-issue)
  - [2. Fork and Clone the Repository](#2-fork-and-clone-the-repository)
  - [3. Create a Branch](#3-create-a-branch)
  - [4. Make Your Changes](#4-make-your-changes)
  - [5. Commit Your Changes](#5-commit-your-changes)
  - [6. Test Your Changes](#6-test-your-changes)
  - [7. Push and Open a Pull Request](#7-push-and-open-a-pull-request)
- [Code Style Guidelines](#code-style-guidelines)
- [Commit Message Guidelines](#commit-message-guidelines)
- [Review Process](#review-process)
- [Community](#community)

---

## Code of Conduct

This project and everyone participating in it is governed by the [Edito Code of Conduct](CODE_OF_CONDUCT.md).  
By participating, you are expected to uphold this code. Please report unacceptable behavior to [your-email@example.com].

---

## How Can I Contribute?

### Reporting Bugs

If you find a bug, please help us by opening an issue using the **Bug Report** template.  
Include as much detail as possible:

- Steps to reproduce the bug
- What you expected to happen vs. what actually happened
- Your operating system and Qt version
- Screenshots if applicable

All bug reports are tracked in [GitHub Issues](https://github.com/Yovsky/Edito/issues).

### Suggesting Enhancements

We welcome feature requests and ideas! Please use the **Feature Request** template when opening an issue.  
Describe your idea, why it would be useful, and how you envision it working.  
Don’t worry if you’re not a developer—your input is just as valuable.

### Your First Code Contribution

Not sure where to start? Look for issues labeled **`good first issue`** or **`help wanted`**.  
These are designed to be beginner-friendly and well-defined. If you’d like to work on something not yet listed, just open an issue to discuss it first—we’ll help you get started.

---

## Development Environment Setup

Edito is built with:

- **C++17** (or later)
- **Qt 6.9** (Widgets module)
- **MinGW 64-bit** compiler (or any Qt-compatible compiler)
- **Hunspell** for spellchecking (already included in `third-party/hunspell`)

Full step-by-step instructions are in **[docs/building.md](docs/building.md)**.  
In short:

1. Install Qt 6.9+ with MinGW 64-bit from the [Qt online installer](https://www.qt.io/download-qt-installer).
2. Clone the repository:  
   `git clone https://github.com/YOUR_USERNAME/Edito.git`
3. Open `CMakeLists.txt` (or `.pro` file) in Qt Creator, configure the kit, and build.

> ⚠️ Ensure the `dictionaries` folder and Hunspell dependencies are correctly linked. See the building guide for troubleshooting.

---

## The Contribution Workflow

### 1. Find or Create an Issue

Before you start coding, check if there’s an [existing issue](https://github.com/Yovsky/Edito/issues) you’d like to work on.  
If none fits, open a new one to describe your proposal. This prevents duplicate work and allows discussion before you invest time.

### 2. Fork and Clone the Repository

Fork the Edito repo to your GitHub account, then clone your fork locally:

```bash
git clone https://github.com/YOUR_USERNAME/Edito.git
cd Edito
git remote add upstream https://github.com/ORIGINAL_OWNER/Edito.git
```

### 3. Create a Branch

Always create a new branch for your work. Use a descriptive name:

```bash
git checkout -b feature/spellcheck-options
# or
git checkout -b fix/crash-on-save
```

### 4. Make Your Changes

Keep changes focused on a single issue. Follow the [Code Style Guidelines](#code-style-guidelines).  
Update documentation if your change affects user-facing behavior (e.g., new menu items, shortcuts, or spellchecking options).

### 5. Commit Your Changes

Write clear, descriptive commit messages (see [Commit Message Guidelines](#commit-message-guidelines)).  
Each commit should represent a logical unit of change.

### 6. Test Your Changes

- Ensure the project builds without errors.
- Run the application and manually verify that your changes work as expected.
- Check that spellchecking still functions correctly if your change involves text handling.
- If you’ve added new functionality, try to test edge cases.

Edito does not yet have an automated test suite—if you’re able to write unit tests, that’s a fantastic contribution!

### 7. Push and Open a Pull Request

Push your branch to your fork:

```bash
git push origin feature/spellcheck-options
```

Go to the original Edito repository on GitHub and open a **Pull Request**.  
You’ll see a PR template—please fill it out completely. Link the related issue using `Closes #X` or `Fixes #X`.

---

## Code Style Guidelines

Consistency makes the codebase easier to read and maintain. We loosely follow the [Qt Coding Style](https://wiki.qt.io/Qt_Coding_Style):

- **Indentation:** 4 spaces, no tabs.
- **Braces:** Same line as the statement (`if (condition) {`).
- **Class names:** `PascalCase` (e.g., `SpellChecker`).
- **Method names:** `camelCase` (e.g., `checkSpelling`).
- **Member variables:** prefix with `m_` or use trailing underscore (e.g., `m_dictionaryPath`).
- **Pointers and references:** `Type* pointer; Type& reference;` (asterisk/ampersand attaches to the type).
- **Comments:** Use `//` for single-line comments, `/* */` for multi-line. Explain *why*, not *what*.

When in doubt, match the style of the surrounding code. If you’re touching a file with a different convention, stick to that file’s convention.

---

## Commit Message Guidelines

We follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <short summary>

[optional body]

[optional footer]
```

**Types:** `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`.  
**Scope (optional):** e.g., `spellcheck`, `ui`, `build`.  

Examples:

- `feat(spellcheck): add option to ignore words with numbers`
- `fix(editor): prevent crash when saving empty file`
- `docs: update README with installation steps`

Keep the summary line under 72 characters. Reference issues in the footer: `Closes #123`.

---

## Review Process

Once you open a pull request:

1. Automated checks may run (if configured).
2. A maintainer will review your changes. They might ask questions or request modifications—this is normal and collaborative.
3. After approval, your PR will be merged. We squash-merge most PRs to keep history clean, so don’t worry about force-pushing to tidy up.

Please be patient; we’re all volunteers. If you don’t hear back in a few days, feel free to politely ping the thread.

---

## Community

Join the discussion!  
- [GitHub Discussions](https://github.com/Yovsky/Edito/discussions)
- Email: mouloudguenane@proton.me

We’re building something together, and every contributor is a valued member of the Edito community.
