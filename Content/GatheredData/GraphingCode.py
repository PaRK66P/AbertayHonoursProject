import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
df = pd.read_csv("PCGDataValues_19_03_2026_06_34_02.csv")

# Create a box plot using Matplotlib
plt.figure(figsize=(8, 6))
plt.boxplot([df[col] for col in df.columns], labels=df.columns, whiskerprops={'linestyle':'--'}, whis=2.0)
plt.title("PCG Evaluation Values Box Plot")
plt.ylabel("Value (0-1)")
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.show()