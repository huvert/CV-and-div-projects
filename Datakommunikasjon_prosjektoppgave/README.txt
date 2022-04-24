
[ESP mikrocontroller] <------> [Server] <---------> [Nettside]
 				   A
				   |
 				   V
 		     [Google Firebase database]

Serveren kan ha tilnærmet ubegrenset mange klienter.
Klientlista på Nettsiden vil bli scrollbar hvis svært mange kobler seg til.
Strukturen i Google Firebase er gjort på en dynamisk måte som gjør dette mulig.

Klienter kan være av 2 typer: [Nettside] eller [ESP mikrocontroller]

Admin-tab i toppmeny inneholder en liten Rick-Roll og er derfor ikke med i rapporten. 