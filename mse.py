import numpy as np
import matplotlib.pyplot as plt

# Discrete Time Unit Step Function
n = np.arange(-5, 6)  # Discrete time vector
u_n = np.heaviside(n, 1)  # Heaviside step function

# Plot discrete time unit step function
plt.figure(figsize=(6, 4))
plt.stem(n, u_n, basefmt=" ", use_line_collection=True)
plt.title('Discrete Time Unit Step Function u[n]')
plt.xlabel('n')
plt.ylabel('u[n]')
plt.grid()
plt.axis([-5, 5, -0.5, 1.5])
plt.show()