import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
df = pd.read_csv("PCGDataValues_14_04_2026_06_31_47.csv")

# Create a box plot using Matplotlib
plt.figure(figsize=(8, 6))
plt.boxplot([df[col] for col in df.columns], labels=df.columns, whiskerprops={'linestyle':'--'}, whis=1.5)
plt.title("PCG Evaluation Values Box Plot")
plt.ylabel("Value (0-1)")
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.show()

q1 = df["PlatformDensity"].quantile(0.25)
q3 = df["PlatformDensity"].quantile(0.75)

iqr = q3 - q1

print("Q1:", q1)
print("Q3:", q3)
print("IQR:", iqr)