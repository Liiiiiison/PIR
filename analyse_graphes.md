# analyse globale des graphes
We can see that in every situation, a '1' is correlated to a minimum power consumption of 0.6mw. 
In the case of a 0, without any other component changing its state at the same time, the consumption stays at idle : 0.4mw.

Whenever another component changes its state at the same clock cycle that we receive a photon, the consumption rises over 0.6mw and it is difficult to say wether a '0' or a '1' was received. During our simulations with non alternating bases, this event occured around 12.5% of '0' receptions.

With fully alternating bases, we can observe the power consumption of the activation/deactivation of de Hadamard's gate is occulting the power consumption of any detector's click, making their power signature undiscernable.

_As our simulation was constently switching base at each observation cycle, it does not reflect a real world scenario where observation base stays the same at least twice in a row, allowing recovery of at least 50% of the key on a single trace without any further analysis_