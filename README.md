# 📈 Black–Scholes Greeks Visualizer API (C++)

This project provides a **C++ API** for computing and visualizing various **option Greeks** under the **Black–Scholes framework** for both **call and put options**.  

It leverages modern C++ features and visualization libraries like [Matplot++](https://github.com/alandefreitas/matplotplusplus) and [ImGui](https://github.com/ocornut/imgui) to produce interactive or static visualizations of option sensitivities.

---

## 🧮 Features

- Compute classical Greeks: **Delta**, **Gamma**, **Vega**, **Theta**, **Rho**  
- Handle both **Call** and **Put** options  
- Visualize Greeks with **Matplot++**  
- Interactive exploration via **ImGui**  
- Modular API design for easy integration into other projects  

---

## 🏗️ Project Setup

To make the project work locally, follow these steps:

1. **Clone this repository**  
   ```bash
   git clone https://github.com/MaxHeu/Greeks_Visualization.git
   cd Greeks_Visualization
2. **Create a folder structure like this:**
'''
your_project/
├── external/
│   ├── matplotplusplus/    # cloned repo (Matplot++)
│   └── imgui/              # cloned repo (ImGui)
├── src/                    # source files
├── include/                # header files
├── CMakeLists.txt
├── main.cpp
└── README.md
'''
3. **Clone required dependencies inside the external/ folder:**
cd external
git clone https://github.com/alandefreitas/matplotplusplus.git
git clone https://github.com/ocornut/imgui.git
cd ..
4. **Build the project as you usually do:**
   mkdir build && cd build
cmake ..
make
5. **Run the executable and enjoy the visualizations!**

## ⚙️ Parameters
The API supports the following Black–Scholes input parameters into sliders. You can configure these parameters directly in the code or via a graphical interface (if enabled).
|   Symbol   | Description                  | Example Value |  Range |
| :--------: | ---------------------------- | :-----------: | :----: |
|   ( S_0 )  | Initial stock price          |     100.0     |  0-999 |
|    ( K )   | Strike price                 |     100.0     |  0-999 |
| ( \sigma ) | Volatility (annualized)      |      0.2      |  0.05-1|
|    ( r )   | Risk-free rate               |      0.05     |  0-0.3 |
|    ( T )   | Time to maturity (in years)  |      1.0      | 0.1-5  |
|    ( q )   | Yield                        |      0.01     | 0-0.3  |
|    (ITM)   | In-the-Money (%)             |      5.0      | 0-99   |
|    (OTM)   | Out-of-the-Money (%)         |      5.0      | 0-99   |

In file _param.txt_, user can modify last three parameters (parameters are case sensitive and should only be separated by coma, not spaces: 
|   Symbol   | Description                  | Example Value               |
| :--------: | ---------------------------- | :------------------------:  |
|   Greeks=  | Types of Greeks computed     |  Delta,Gamma,Vega,Rho,Theta |
|   Options= | Option Types computed        |     Call and/or Put         |
|   Plots=   | Different visualization      |  Simple or 3D or Moneyness  |

_Example of usage_ : 
Greeks=Delta,Gamma
Options=Call,Put
Plots=Simple

Greeks=Delta,Gamma,Vega
Options=Call
Plots=3D

## 📊 Example Visualizations
| Visualization              | Description                                                     |
| :------------------------- | :-------------------------------------------------------------- |
| ![Delta Surface](fig1.png) | Delta variation with respect to underlying price and volatility |
| ![Gamma Surface](fig2.png) | Gamma heatmap across different strikes and maturities           |

## 🧰 Dependencies

C++17 or newer

Matplot++

ImGui

CMake ≥ 3.10

🧑‍💻 Author

Developed by _Maxime Heuse_ as a practical tool for option analysis and visualization under the Black–Scholes model.
