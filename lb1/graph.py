import matplotlib.pyplot as plt

primes = [2,3,5,7,11,13,17,19,23,29,31]
iterations = [2,6,19,92,1776,5290,43801,103275,535267,4591530,8243190]

plt.figure(figsize=(14, 7))

plt.plot(primes, iterations, 'bo-', markersize=8,
         markerfacecolor='yellow', linewidth=2, markeredgecolor='black')

plt.yscale('linear')
plt.title('Экспоненциальный рост итераций для простых N', fontsize=14)
plt.xlabel('Простое число (N)', fontsize=12)
plt.ylabel('Количество итераций', fontsize=12)

plt.grid(True, which="both", ls="--", alpha=0.7)
plt.xticks(primes, fontsize=10)
plt.yticks(fontsize=10)

plt.ticklabel_format(axis='y', style='scientific', scilimits=(0,0))

plt.annotate(f'{iterations[0]}', (primes[0], iterations[0]),
             textcoords="offset points", xytext=(10,-10),
             ha='center', va='top', fontsize=10)

plt.annotate(f'{iterations[-1]:.1e}', (primes[-1], iterations[-1]),
             textcoords="offset points", xytext=(-10,10),
             ha='right', va='bottom', fontsize=10,
             arrowprops=dict(arrowstyle="->"))

plt.tight_layout()
plt.show()