# 🌟 **System Programming** 🌟

Welcome to the **System Programming** repository! This repository serves as a comprehensive guide to understanding fundamental concepts of system programming, focusing on assemblers and macros. 📚✨

---

## 🛠️ **What is an Assembler?**
An **assembler** is a system program that converts assembly language into machine code. It bridges the gap between human-readable code and machine-readable instructions. 
- Input: Assembly language code 💡
- Output: Machine code ⚙️

---

## 🌀 **Single Pass Assembler**
A **Single Pass Assembler** processes the source code in **one pass**. It performs the translation of assembly instructions and symbol resolution simultaneously.  
### 🔑 **Key Features:**
- Faster translation ⚡
- Limited error handling ❌
- Suitable for simpler assembly programs 🎯

---

## 🌐 **Two Pass Assembler**
A **Two Pass Assembler** processes the source code in **two distinct passes**:  
1️⃣ **Pass 1:** Analyzes and collects information.  
2️⃣ **Pass 2:** Generates machine code based on the data collected.

### 🎯 **Advantages:**
- Handles forward references ✅  
- Provides better error diagnostics 🛡️  

---

## 📋 **Pass 1 of a Two Pass Assembler**
The **first pass** of a two-pass assembler performs the following tasks:
- Constructs the **Symbol Table** 🗂️
- Calculates **Instruction Lengths** 📏
- Detects **Labels and Addresses** 📍

### 🛠️ **Outputs:**
- Symbol Table  
- Intermediate Code  

---

## 🧩 **Pass 2 of a Two Pass Assembler**
The **second pass** utilizes the outputs from the first pass to:
- Translate intermediate code into **Machine Code** 🔄  
- Perform **Address Binding** 📌  
- Generate the **Object Code** 🖨️  

---

## 📝 **Macro Definition**
A **macro** is a sequence of instructions grouped under a name, used to simplify repetitive tasks.  
### ✨ **Benefits of Macros:**
- Reduces code duplication 🌀  
- Enhances code readability 📖  
- Improves development efficiency 🚀  

---

## 🔄 **Macro Expansion**
The **Macro Expansion** process replaces a macro name with the actual sequence of instructions it represents. It ensures the assembly code integrates seamlessly during preprocessing.  

### 🛠️ **Steps:**
1. Locate macro definitions in the code. 🔍  
2. Replace macro calls with corresponding code. 🔄
   
---

🌟 **Happy Learning and Coding!** 🌟
