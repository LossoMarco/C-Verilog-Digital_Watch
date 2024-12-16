#include <stdint.h>

// Definizione degli indirizzi di memoria
#define ENCODER_ADDR 0xff200040
#define DISPLAY_ADDR 0xff200010
#define TIMER_ADDR 0xff200020
#define BUTTON_ADDR 0xff200080
#define TIMER_LAMP_ADDR 0xff200030
#define TIMER_CRON 0xff200090

// Inizializzazione orologio come variabili globali
unsigned int sec = 0;
unsigned int decisec = 0;
unsigned int min = 0;
unsigned int decimin = 0;
unsigned int ore = 0;
unsigned int deciore = 0;

// Inizializzazione valore precedente del clock a 1Hz
int old_click = 0;

// Dichiarazione funzioni
unsigned int aggiornamento_orologio (volatile int *new_click_ptr, int formato_ora);
void modifica_orologio (volatile int *encoder_ptr, volatile int *clk_2Hz_ptr, unsigned int *sec, unsigned int *decisec, unsigned int *min, unsigned int *decimin, unsigned int *ore, unsigned int *deciore, volatile int *display_7_seg_ptr, int stato, int formato_ora);
void modifica_sec_min (int encoder_reg_curr, int encoder_reg_prev, unsigned int *valore, unsigned int *decivalore);
void modifica_ore (int encoder_reg_curr, int encoder_reg_prev, unsigned int *ore, unsigned int *deciore, int formato_ora);
int seleziona_formato_ora(volatile int *encoder_ptr, volatile int *display_7_seg_ptr, int formato_corrente);
unsigned int aggiorna_cronometro (volatile int *clk_100Hz_ptr, volatile int *button_ptr, unsigned int *centesimi, unsigned int *secondi, unsigned int *minuti, int *cronometro_attivo, int visualizzazione);
void aggiorna_display(volatile int *display_7_seg_ptr, int visualizzazione, unsigned int output_display_orologio, unsigned int output_display_cronometro);

int main() {
    // Puntatore che punta all'indirizzo di memoria dell'encoder rotativo
    volatile int *encoder_ptr = ENCODER_ADDR;
    
    // Puntatore che punta all'indirizzo di memoria del display a 7 segmenti
    volatile int *display_7_seg_ptr = DISPLAY_ADDR;

    // Puntatore che punta all'indirizzo di memoria del valore del timer a 1 Hz
    volatile int *clk_1Hz_ptr = TIMER_ADDR;

    // Puntatore che punta all'indirizzo di memoria del valore del pulsante dell'encoder
    volatile int *button_ptr = BUTTON_ADDR;

    // Puntatore che punta all'indirizzo di memoria del valore del timer a 2 Hz per far lampeggiare durante l'impostazione dell'orologio
    volatile int *clk_2Hz_ptr = TIMER_LAMP_ADDR;

    // Puntatore che punta all'indirizzo di memoria del valore del timer a 100 Hz per il cronometro
    volatile int *clk_100Hz_ptr = TIMER_CRON;

    // variabile uscita per display a 7 segmenti
    unsigned int output_display = 0xaaaaaaaa;  //inizializzazione tutto spento
    *(display_7_seg_ptr) = output_display;

    // variabili per l'orologio e per il cronometro
    unsigned int output_display_orologio = 0xaaaaaaaa;  //inizializzazione tutto spento
    unsigned int output_display_cronometro = 0xaaaaaaaa;  //inizializzazione tutto spento

    // Inizializzazione del formato di visualizzazione delle ore
    int formato_ora = 24;

    // Inizializzazione variabili cronometro
    unsigned int centesimi_cronometro = 0;
    unsigned int secondi_cronometro = 0;
    unsigned int minuti_cronometro = 0;
    int cronometro_attivo = 0;

    // Variabile per tracciare la visualizzazione corrente (orologio o cronometro)
    int visualizzazione = 0; // 0: orologio, 1: cronometro

    static int stato = 0;  // 0: normale, 1: modifica secondi, 2: modifica minuti, 3: modifica ore

    static int encoder_reg_prev = 0;  // inizializzazione stato encoder per confronto

    // Ciclo infinito del programma
    while(1)
    {   
        static int pulsante_precedente = 1;   // il pulsante non premuto scrive 1 nel registro di memoria, 0 se premuto
        int pulsante_corrente = *button_ptr;  // lettura registro pulsante

        // Gestione switch tra orologio e cronometro con l'encoder
        int encoder_reg_curr = *encoder_ptr / 4; // Valore dell'encoder


        if (pulsante_precedente == 1 && pulsante_corrente == 0) { // Rileva pressione del pulsante
            if (visualizzazione == 0){
                stato = (stato + 1) % 5;  // aggiornamento dello stato
            }
        }

        pulsante_precedente = pulsante_corrente;  // aggiornamento del pulsante
        
        if(stato == 1){ // se lo stato è 1 passo alla selezione del formato dell'ora prima di poterla modificare
            formato_ora = seleziona_formato_ora(encoder_ptr, display_7_seg_ptr, formato_ora);
        } else if(stato != 0 && stato != 1){  // se lo stato è diverso da 0 o da 1 passo alla modifica delle variabili
            
            modifica_orologio(encoder_ptr, clk_2Hz_ptr, &sec, &decisec, &min, &decimin, &ore, &deciore, display_7_seg_ptr, stato, formato_ora);

            encoder_reg_prev = encoder_reg_curr;  // Serve aggiornare in modo che non cambi visualizzazione in automatico quando esco dal set dell'orologio

        } else {  // altrimenti rimango nello stato normale

            if (encoder_reg_curr != encoder_reg_prev) {
            visualizzazione = !visualizzazione; // Toggle visualizzazione tra orologio e cronometro
            }

            encoder_reg_prev = encoder_reg_curr;  // aggiornamento registro encoder

            output_display_orologio = aggiornamento_orologio(clk_1Hz_ptr, formato_ora); // risultato visualizzazione orologio
            output_display_cronometro = aggiorna_cronometro(clk_100Hz_ptr, button_ptr, &centesimi_cronometro, &secondi_cronometro, &minuti_cronometro, &cronometro_attivo, visualizzazione); // risultato visualizzazione cronometro
            
            // Aggiorna il display in base alla visualizzazione corrente 
            aggiorna_display(display_7_seg_ptr, visualizzazione, output_display_orologio, output_display_cronometro);
        }
    }
}

unsigned int aggiornamento_orologio (volatile int *new_click_ptr, int formato_ora) {
    
    int new_click = *(new_click_ptr);  // rilevo nuovo valore del registro a 1Hz
    unsigned int output_display; // inizializzazione variabile uscita

    if (old_click != new_click) {  // se il valore del clk a 1 Hz è cambiato aggiorno l'ora
        sec++;

        if (sec >= 10) {
            sec = 0;
            decisec++;

            if (decisec >= 6) {
                decisec = 0;
                min++;

                if (min >= 10) {
                    min = 0;
                    decimin++;

                    if (decimin >= 6) {
                        decimin = 0;
                        ore++;

                        if (ore >= 10) {
                            ore = 0;
                            deciore++;
                        }
                    }
                }
            }
        }

        // Verifica il caso speciale per ore >= 24 o 12
        if (formato_ora == 24) {
            if (deciore >= 2 && ore >= 4) {
                deciore = 0;
                ore = 0;
            }
        } else { // formato 12 ore
            if (deciore >= 1 && ore >= 2) {
                // Gestisce il passaggio da 11:59:59 a 12:00:00
                ore = 0;
                deciore = 0;
            }
        }
    }

    old_click = new_click; // aggiornamento click a 1 Hz

    if (deciore != 0){  // discriminazione del caso in cui le ore siano maggiori di 10 in modo da tenere spento il display più significativo
        output_display = 0xAA000000 | (deciore << 20) | (ore << 16) | (decimin << 12) | (min << 8) | (decisec << 4) | sec;
    } else {
        output_display = 0xAAA00000 | (deciore << 20) | (ore << 16) | (decimin << 12) | (min << 8) | (decisec << 4) | sec;
    }

    return output_display;  // restituisce il valore di output_display
}

void modifica_orologio(volatile int *encoder_ptr, volatile int *clk_2Hz_ptr, unsigned int *sec, unsigned int *decisec, unsigned int *min, unsigned int *decimin, unsigned int *ore, unsigned int *deciore, volatile int *display_7_seg_ptr, int stato, int formato_ora) {
    
    // Inizializzazione valore precedente del registro dell'encoder rotativo
    static int encoder_reg_prev = 0;

    int encoder_reg_curr = *encoder_ptr / 4; // rileva valore dell'encoder rotativo, diviso per 4 perchè ad ogni click aumenta di 4 il suo valore
    int clk_2Hz_curr = *clk_2Hz_ptr;  // rileva del valore del registro del clock a 2 Hz utilizzato per avere un lampeggio più veloce

    unsigned int output_display = *(display_7_seg_ptr);  // rilevo il valore dell'ora presente nei display attualmente

    if ((clk_2Hz_curr % 2) == 0){  // Qualdo il valore nel registro è pari tengo i display spenti solo della variabile sotto modifica
        // Lampeggia i display per il valore che si sta modificando

        switch (stato) {
            case 2:
                // Lampeggia i secondi
                output_display &= 0xFFFFFF00; // And bit a bit per mettere a 0 solo i valori dei secondi
                output_display |= 0x000000AA; // Or bit a bit per spegnere solo i display dei secondi

                // Permette la modifica secondi nel mentre che è spento
                modifica_sec_min(encoder_reg_curr, encoder_reg_prev, sec, decisec);

                break;

            case 3:
                // Lampeggia i minuti
                output_display &= 0xFFFF00FF; // And bit a bit per mettere a 0 solo i valori dei minuti
                output_display |= 0x0000AA00; // Or bit a bit per spegnere solo i display dei minuti

                // Permette la modifica minuti nel mentre che è spento
                modifica_sec_min(encoder_reg_curr, encoder_reg_prev, min, decimin);

                break;

            case 4:
                // Lampeggia le ore
                output_display &= 0xFF00FFFF; // And bit a bit per mettere a 0 solo i valori dei ore
                output_display |= 0x00AA0000; // Or bit a bit per spegnere solo i display dei ore

                // Permette la modifica ore nel mentre che è spento
                modifica_ore (encoder_reg_curr, encoder_reg_prev, ore, deciore, formato_ora);

                break;

            default:
                // Stato normale
                break;
        }

        *(display_7_seg_ptr) = output_display; // Aggiornamento del display

    } else {  // Permetto la modifica delle variabioli am mantenendo i display accesi

    // Logica di modifica delle variabili
    switch (stato) {
        case 2:
            // Modifica secondi
            modifica_sec_min(encoder_reg_curr, encoder_reg_prev, sec, decisec);

            break;

        case 3:
            // Modifica minuti
            modifica_sec_min(encoder_reg_curr, encoder_reg_prev, min, decimin);

            break;

        case 4:
            // Modifica ore
            modifica_ore (encoder_reg_curr, encoder_reg_prev, ore, deciore, formato_ora);
            
            break;

        default:
            // Stato normale
            break;
    }


    // discrimnazione tra ore maggiori di 10 o no
    if (*deciore != 0){
        output_display = 0xAA000000 | (*deciore << 20) | (*ore << 16) | (*decimin << 12) | (*min << 8) | (*decisec << 4) | *sec;
    } else {
        output_display = 0xAAA00000 | (*deciore << 20) | (*ore << 16) | (*decimin << 12) | (*min << 8) | (*decisec << 4) | *sec;
    }

    *(display_7_seg_ptr) = output_display; // Aggiornamento il display

    }

    encoder_reg_prev = encoder_reg_curr; // Aggiorna il valore precedente dell'encoder
}

void modifica_sec_min(int encoder_reg_curr, int encoder_reg_prev, unsigned int *valore, unsigned int *decivalore){

    if (encoder_reg_curr > encoder_reg_prev) {
        if (*valore < 9) {
            (*valore)++;
        } else {
            *valore = 0;
            if (*decivalore < 5) {
                (*decivalore)++;
            } else {
                *decivalore = 0;
            }
        }
    } else if (encoder_reg_curr < encoder_reg_prev) {
        if (*valore > 0) {
            (*valore)--;
        } else {
            *valore = 9;
            if (*decivalore > 0) {
                (*decivalore)--;
            } else {
                *decivalore = 5;
            }
        }
    }
}

void modifica_ore (int encoder_reg_curr, int encoder_reg_prev, unsigned int *ore, unsigned int *deciore, int formato_ora) {
    
    // Verifica iniziale per convertire automaticamente le ore se il formato è 12 ore
    if (formato_ora == 12) { 
        if (*deciore >= 1 && *ore >= 2) { 
            // Converte le ore oltre le 12 al formato da 12 ore 
            int ore_totali = *deciore * 10 + *ore; ore_totali -= 12; 
            *deciore = ore_totali / 10; // ricavo le decine 
            *ore = ore_totali % 10; // ricavo le unità
        }
    }
    
    if (encoder_reg_curr > encoder_reg_prev) {
        if (*ore < 9) {
            (*ore)++;
        } else {
            *ore = 0;
            if (*deciore < 2) {
                (*deciore)++;
            } else {
                *deciore = 0;
            }
        }

        // Verifica il caso speciale per ore tra 20 e 23 (formato 24 ore)
        if (formato_ora == 24) {
            if (*deciore >= 2 && *ore >= 4) {
                *ore = 0;
                *deciore = 0;
            }
        } else { // formato 12 ore
            if (*deciore >= 1 && *ore >= 2) {
                // Gestisce il passaggio da 11:59 a 12:00
                *ore = 0;
                *deciore = 0;
            }
        }
    } else if (encoder_reg_curr < encoder_reg_prev) {
        if (*ore > 0) {
            (*ore)--;
        } else {
            *ore = 9;
            if (*deciore > 0) {
                (*deciore)--;
                // Verifica il caso speciale per ore tra 20 e 23 (formato 24 ore)
                if (formato_ora == 24) {
                    if (*deciore == 2 && *ore >= 4) {
                        *ore = 3;
                    }
                } else { // formato 12 ore
                    if (*deciore >= 1 && *ore >= 2) {
                        // Gestisce il passaggio da 12:00 a 11:59
                        *ore = 1;
                        *deciore = 1;
                    }
                }
            } else {
                if (formato_ora == 24) {
                    *deciore = 2;
                    *ore = 3;
                } else {
                    *deciore = 1;
                    *ore = 1;
                }
            }
        }
    }
}

int seleziona_formato_ora(volatile int *encoder_ptr, volatile int *display_7_seg_ptr, int formato_corrente) {
    int encoder_reg_curr = *encoder_ptr / 4; // Valore dell'encoder
    static int encoder_reg_prev = 0;

    // Aggiorna display per mostrare il formato selezionato
    unsigned int output_display;

    if (formato_corrente == 24) {
        output_display = 0xAAABAA24; // Valore arbitrario per mostrare "24"
    } else {
        output_display = 0xAAABAA12; // Valore arbitrario per mostrare "12"
    }
    *display_7_seg_ptr = output_display;

    // Cambia formato ora tra 12 e 24 ad ogni click dell'encoder
    if (encoder_reg_curr != encoder_reg_prev) {
        if (formato_corrente == 24) {
            formato_corrente = 12;
        } else {
            formato_corrente = 24;
        }
        encoder_reg_prev = encoder_reg_curr;
    }
    
    return formato_corrente;
}

unsigned int aggiorna_cronometro (volatile int *clk_100Hz_ptr, volatile int *button_ptr, unsigned int *centesimi, unsigned int *secondi, unsigned int *minuti, int *cronometro_attivo, int visualizzazione) {
    
    static int old_clk_100Hz = 0;  // Inizializzazione valore precedente del clock a 100 Hz
    int new_clk_100Hz = *(clk_100Hz_ptr);  // Rilevo nuovo valore del registro a 100 Hz
    unsigned int output_display; // Inizializzazione variabile uscita

    // Controllo stato del pulsante dell'encoder
    static int pulsante_precedente = 1;
    int pulsante_corrente = 1;
    static unsigned int tempo_premuto = 0;

    if(visualizzazione == 1) {  // rileva il pulsante solo se siamo nella visualizzazione del cronometro
        pulsante_corrente = *button_ptr;

        if (pulsante_precedente == 1 && pulsante_corrente == 0 && visualizzazione == 1) { // Rileva pressione del pulsante 
            tempo_premuto = 0; // Reset del tempo di pressione 
        } 
    }
    
    if (pulsante_corrente == 0) {
        if (new_clk_100Hz != old_clk_100Hz) {            
            tempo_premuto++; // Incrementa il tempo di pressione 
        }
    } else if (pulsante_precedente == 0 && pulsante_corrente == 1) { // Rileva rilascio del pulsante 
        if (tempo_premuto >= 100) { // Pulsante premuto per più di un secondo (100 tick di 100Hz) 
            *centesimi = 0; 
            *secondi = 0; 
            *minuti = 0; 
            *cronometro_attivo = 0; // Reset del cronometro 
        } else { // Pulsante premuto per meno di un secondo 
            *cronometro_attivo = !(*cronometro_attivo); // Toggle stato del cronometro 
        } 

        tempo_premuto = 0; // Reset del tempo di pressione 
    }

    pulsante_precedente = pulsante_corrente; // Aggiornamento stato del pulsante

    if (new_clk_100Hz != old_clk_100Hz && *cronometro_attivo) {  // Se il valore del clk a 100 Hz è cambiato e il cronometro è attivo, aggiorno il cronometro
        (*centesimi)++;

        if (*centesimi >= 100) {
            *centesimi = 0;
            (*secondi)++;

            if (*secondi >= 60) {
                    *secondi = 0;
                    (*minuti)++;
            }
        }
    }

    // Reset del cronometro se raggiunge 59:59:99
    if (*minuti >= 59) {
        *minuti = 0;
        *secondi = 0;
        *centesimi = 0;
    }

    old_clk_100Hz = new_clk_100Hz; // Aggiornamento click a 100 Hz

    // Costruzione output display
    output_display = (*minuti >= 10 ? *minuti / 10 * 0x100000 : 0xAAA00000)  // Gestione display minuti decine
                | (*minuti == 0 ? 0xAA0000 : *minuti % 10 * 0x010000)  // Gestione display minuti unità
                | (*minuti == 0 && *secondi < 10 ? 0xAAA000 : *secondi / 10 * 0x001000)  // Gestione display secondi decine
                | (*secondi % 10 * 0x000100)  // Gestione display secondi unità
                | (*centesimi / 10 * 0x000010) // Gestione display decimi
                | (*centesimi % 10 * 0x000001); // Gestione display centesimi

    return output_display;  // Restituisce il valore di output_display
}

void aggiorna_display(volatile int *display_7_seg_ptr, int visualizzazione, unsigned int output_display_orologio, unsigned int output_display_cronometro) {
    if (visualizzazione == 0) {
        *display_7_seg_ptr = output_display_orologio; // Visualizza l'orologio
    } else {
        *display_7_seg_ptr = output_display_cronometro; // Visualizza il cronometro
    }
}