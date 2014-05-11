	Projekt ICP 2014 Bludiste
	Client-server hra
    
    Autori:
		Stefan Marticek xmarti62
        Oliver Nemcek xnemce03
	
	Ovladanie hry GUI:
        Rezim celej obrazovky (fullscree / normal) sa prepina dvojklikom na herny plan.
        Priblizenie alebo oddialenie sa realizuje kolieskom mysi.
        Checkbox "Center on me" na obrazovke hry vystreduje hraca na obrazovke.
        Ostatne sucasti su popisane v sekcii "Vlastne upravy zadania" alebo su natolko intuitivne ze nepotrebuju popis.
        
	Textove prikazy a klavesove skratky(implementovane aj v GUI aj v konzolovej verzii): 
        GO = W
        STOP = S
        LEFT = A
        RIGHT = D
        TAKE = E
        OPEN = Q
        
    Vyznam znakov pre konzoloveho klienta:
        Hraci su znazorneni sipkami.
        Strazcovia su znazorneni pismenom X.
        Zatvorene brany su znazornene pismenom G.
        Kluce su znazrnene pismenom K.
        Poklad je znazorneny pismenom F.
        Ostatne casti prostredia su intuitivne rozpoznatelne.
	
    Vlastne upravy zadania:
        Ovladanie terminaloveho klienta je implementovane len pomocou klavesovych skratiek.
        Do hry sme pridali timeout, ktory urcuje dobu cakania na spustenie hry (vhodne pre vyckanie
        na pripojenie vsetkych hracov a na pociatocny nahlad hry).
        Hreci nie su odliseni farbami ale texturovymi vzormi.
        Hra po smrti vsetkych hracov nekonci, naopak stale je mozne sa do hry pripojit,
        pokial ju vsetci hraci este neopustili.
        Po vyhre sa zastavi casomer jej trvania, ale jednotlivym hracom pocitadlo casu stale bezi.
        
    Spustanie aplikacie:
        server:
            ./bludiste2014-server <port>
        klient - GUI:
            ./bludiste2014
        klient - konzolova verzia:
            ./bludiste2014-cli <hostname> <port>
    
	