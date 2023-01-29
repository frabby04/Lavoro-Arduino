#include <SPI.h> //SerialPeripheralInterface
#include <Ethernet.h>
#include "LedControl.h" //Questa libreria permette il controllo della matrice di LED.

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xD4, 0xB1}; // Indirizzo MAC della scheda di rete di Arduino
byte ip[] = {192, 168, 1, 75};                     // Indirizo IP di Arduino assegnato staticamente
EthernetServer server(80);                         // Creo un server ETH sulla porta 80
LedControl lc = LedControl(7, 6, 5, 1);            // Istanzia un oggetto della classe "LedControl" chiamato "lc"

void scrivi_S();

void setup()
{

  /*
  Richiamo il metodo ".shutdown(indiceDevice,statoDelLED)" per far uscire la matrice di LED dalla modalità di risparmio energetico.
  */
  lc.shutdown(0, false);

  lc.setIntensity(0, 8); // Imposto l'intensità dei LED su valori medi.

  lc.clearDisplay(0); // La matrice di LED viene resettata.

  scrivi_S();
  delay(1000);
  lc.clearDisplay(0);

  Serial.begin(9600); // Inizializzo la porta seriale per poter leggere informazioni sul monitor seriale.

  /*
  Inizializzo il web server, passando come parametri
  l'indirizzo mac della scheda di rete e l'indirizzo IPv4
  assegnato staticamente.
  */
  Ethernet.begin(mac, ip);
  /*
  L'istanza "server" della classe "EthernetServer" richiama il metodo begin
  attivando il server che resta in attesa di richieste da parte di un client.
  */
  server.begin();
  Serial.print("L'ip del server è:  ");
  Serial.println(Ethernet.localIP()); // Utilizzo il metodo statico "localIP" della classe "Ethernet" che ritorna l'indirizzo IP del server.
}

void loop()
{

  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("Un client si è connesso");
    Serial.println("");
    // Comincio ad effettuare un controllo sulla richiesta HTTP, assicurandomi che essa sia terminata.
    /*
    Questa variabile booleana è necessaria perché si possa assicurare la correttezza
    della richiesta HTTP inviata dal client, visto che essa prevede un CRLF successivo a tutte le intestazioni.
    */
    bool controlloRichiesta = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        // Stampo la richiesta HTTP inviata dal client sul monitor seriale.
        Serial.write(c);
        /*
        Se l'ultimo carattere della richiesta HTTP è uguale al caratteri di new line
        allora il server puo' inviare una risposta al client.
        */
        if (c == '\n' && controlloRichiesta)
        {

          Serial.println("INVIO DELLA RISPOSTA");

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          /*
          Dando come valore "close" all'intestazione "Connection" la connessione con il client non persiste.
          */
          client.println("Connection: close");
          /*
          Richiamo il metodo ".println()" affinché sia presente il CRLF alla
          fine della risposta HTTP.
          */
          client.println();

          /*
          Dopo il CRLF, stampo il codice HTML che sarà contenuto nell'entity body della risposta.
          */
          client.println("<!DOCTYPE html>");
          client.println("<html lang='en'>");
          client.println("<head>");
          client.println("<meta charset='UTF-8'>");
          client.println("<meta http-equiv='X-UA-Compatible' content='IE=edge'>");
          client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
          client.println("<title>Home</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1 style='color: rgb(66, 66, 66); font-size: 80px; text-align: center;'>La pagina è stata ricevuta</h1>");
          client.println("<p style='color: salmon; font-size: 40px; text-align: center;'>Il Led mostra la lettera 'S' che indica la parola 'sent'</p>");
          client.println("</body>");
          client.println("</html>");
          scrivi_S(); // Richiamo il metodo per mostrare la lettere S sulla matrice di LED.
          break;
        }
        if (c == '\n')
        { // Il carattere di nuova riga è presente, la richiesta è terminata.
          controlloRichiesta = true;
        }
        else if (c != '\r')
        { // La richiesta non è completa, l'ultima riga non è vuota.
          controlloRichiesta = false;
        }
      }
    }

    delay(1);      // Fornisco al browser 1 millisecondo per ricevere la risposta.
    client.stop(); // Termino la connessione.
    Serial.println("Il client è stato disconnesso.");
  }
}

void scrivi_S()
{ // Metodo utilizzato accendere la matrice di LED in modo da mostrare la lettera S.
  byte letter[8] = {
      B00000000,
      B10000110,
      B10001001,
      B10001001,
      B10010001,
      B10010001,
      B01100001,
      B00000000};
  lc.setRow(0, 0, letter[0]);
  lc.setRow(0, 1, letter[1]);
  lc.setRow(0, 2, letter[2]);
  lc.setRow(0, 3, letter[3]);
  lc.setRow(0, 4, letter[4]);
  lc.setRow(0, 5, letter[5]);
  lc.setRow(0, 6, letter[6]);
  lc.setRow(0, 7, letter[7]);
}
