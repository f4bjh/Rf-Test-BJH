[33mcommit 058a500a0957bc8da0f74a9fd1bfb061785377c9[m[33m ([m[1;36mHEAD[m[33m -> [m[1;32m131-time-stamping-counter[m[33m, [m[1;31morigin/131-time-stamping-counter[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 18 08:12:33 2026 +0100

    131 - [meas] insert MEASURMENT_TASK_WAKE_UP_TICK at 250ms for all measurments task to be used in vTaskDelay. Decrease wake up tick time of http server to 100ms. This is to be sure to let http_server getting and sending data from a measurment task. Else, could have some instability

[33mcommit 8122a92e56165dfa4cc43c3ba5f1e08091345460[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 18 07:57:01 2026 +0100

    131 - [meas/freq] useless check at init of frequencymeter. checking version of fpga, and checing taht FPGA is not in error state is not a so good idea. because task of read status will not show any value in case of mismatch between version or in case of error in FPGA. Soon or later it will be a pain

[33mcommit 75f4b8d173a5c19379e13b475ed600741bb2b75e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 18 07:49:06 2026 +0100

    131 - [pages/frequencymeter] FPGA version reformated into a major.minor

[33mcommit 95a61fe3945b5854490d0a6a90ddbd8ab941be70[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 18 07:48:41 2026 +0100

    131 - [meas/freq] FPGA version reformated into a major.minor

[33mcommit a6a2e48e1ff2d6a78c50b3a1f5693ee5b8bfe03f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 18 07:48:20 2026 +0100

    131 - [freq_front_end/reciproq_freq_meas] FPGA version reformated into a major.minor

[33mcommit 8f211ec6fc63a261b07d889037bca167f813d981[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 17 00:06:23 2026 +0100

    131 - [http_server] add a measurement of FPGA read status in html

[33mcommit 097d33a075b459ae00f5cc174dc07c82f6529257[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 17 00:06:04 2026 +0100

    131 - [meas/freq] add a measurement of FPGA read status

[33mcommit dbfd8667368b4e059d668bbeee6de20c627ab8f6[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Mar 16 08:22:09 2026 +0100

    131 - [meas/freq] clean code

[33mcommit 3f1e87dffd3b13debec7de5fdd7cd21cd445fc6a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 14 12:08:22 2026 +0100

    131 - [main] FIX MISO configuration in spi_bus_config_t
    
    First, set mio_io_num to GPIO13, as expected.
    
    But, this is not enough.
    Fix an incorrect initialization of spi_bus_config_t where the field
    data2_io_num was specified twice. This caused the structure layout to
    be misinterpreted at runtime and resulted in miso_io_num being set to -1.
    
    With miso_io_num = -1, the SPI driver disables the MISO line, so the
    ESP32 always read 0x00000000 even though the FPGA was correctly driving
    MISO (verified on the oscilloscope).
    
    Remove the duplicated field and use a minimal SPI bus configuration.
    SPI communication with the FPGA now works correctly.

[33mcommit d3fa3ccef0df410029bee9af39361ab1339e763b[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 13 19:33:05 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] SPI clk frequency on ESP32 is 1MHz

[33mcommit d529b6f61019f1e862029ae33ce44159a42b653a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 13 19:30:04 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas] abstraction of SPI clk frequency in testbench

[33mcommit 89d746583a0b951c1c6940ba83f8dcdac65933a5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Mar 12 00:23:09 2026 +0100

    131 - [meas/freq/] add RX bytes count in the SPI total frame. Else ESP32 seems to not pulse clock anymore at the of the TX on MOSI, and FPGA cannot reply to the ESP32 request. Set to dummy bytes on MOSI during these added bytes. rxlength can be set to 0, because by default it will be set back to the same length as t.length. Also, in spi_transaction_t, length of data are in bits, not in bytes

[33mcommit 864841a15b444c7a1c0a4a6eedd35d77e40b8f3a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Mar 12 00:15:20 2026 +0100

    131 - [how-to/] complete for_developpers.md with some help about vivado use

[33mcommit c5a9c641b0fc54ccf8eade80d861acf7bdf20c8d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 19 23:19:46 2026 +0100

    131 - [meas/freq/] add read status cmd in freq driver

[33mcommit f964df2155447abdf97bc0eb7ba8887d641ac4d2[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 19 22:31:28 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] by default, in spi_decode, set to logic 0 all signals that are pulsed in output

[33mcommit 0d572cef2aa3cba63e1b6bdcf556ffee36b1708b[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Jan 17 15:59:32 2026 +0100

    131 - [meas/freq/] adapt freq driver to new FPGA

[33mcommit 30e34bfa3f289e1d1d41d8f28e001c59c0fe555f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 12 22:28:46 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] to be able to generate bitstream, need to assign signal in xdc. means also it will have to be manage by sw

[33mcommit 8b82a615b6d05486ca15dcd1d63cafd9d2dc78ba[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 13 19:16:32 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] unusefull wait for data coming. and also, can even false results in RX data

[33mcommit eda330b00f3adc53f17d46bdf04489436b774dd9[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 13 19:15:25 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] unusefull signal

[33mcommit 2bca965d5c2ef06e2d2a7788b854e6254e6fd707[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 12 22:08:43 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] remove signal used only by testbench to be synthesizable and implementable

[33mcommit ab789f6e1a3f6ab71a76139796133e87fdd63850[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 12 21:29:52 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] ajoute un signal de detection de fifo full

[33mcommit 40855356478dd0c68524dfc03b04f1fc73daea33[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 12 21:15:42 2026 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] split la partie decodage et la partie generation de signal SPI

[33mcommit 5bd1bd0ed295f828b95d3e4420c82f35894241de[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Dec 20 11:13:56 2025 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] implement a fifo in preparation
    of sending several words of 32 bits

[33mcommit 1b616d820282d25ba13b9b7cd251d4cc575d0988[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Dec 17 21:59:37 2025 +0100

    131 - [hw/freq_front_end/reciproq_freq_meas/] add MISO in spi_slave to send data from FPGA to ESP32

[33mcommit f2b19113b51a4d02e8efa632888c429838dd35fa[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 12 17:09:06 2025 +0200

    131 - [hw/freq_front_end/reciproq_freq_meas/] reteste le bus SPI et connecte la sortie NCO à l'entrée du fréquencemètre dans le testbench

[33mcommit 5a8f6910fd02e7f426fe1a29028db3ace310f269[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 12 16:50:11 2025 +0200

    131 - [hw/freq_front_end/reciproq_freq_meas/] pour être synthetisable, TIMEOUT_MS dépasse la plage du type integer car en VHDL, les expressions dans les constantes sont évaluées comme des integer avant d’être converties

[33mcommit 762effa2f739b9d37e6e2d5ccb1158446093a003[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 18:40:24 2025 +0200

    131 - [hw/freq_front_end/reciproq_freq_meas/] ajout du composant freq_counter et calc_freq basé sur le principe de compteur reciproque
    
    - Séparation du fréquencemètre réciproque en deux modules :
      - **freq_counter.vhdl** : réalise la mesure de durée entre deux événements du signal à mesurer.
        - Compte les ticks de l’horloge de référence (FREF_HZ) pendant N cycles du signal d’entrée.
        - Fournit start_tick, end_tick et N_counted.
        - Implémente une machine d’état gérant les séquences reset → attente front montant → mesure → fin.
      - **calc_freq.vhdl** : calcule la fréquence mesurée à partir des valeurs de freq_counter.
        - f_est_int : partie entière de la fréquence.
        - f_est_frac : partie fractionnaire sur FRAC_BITS bits.
        - df_q16_16 : pas de quantification (Hz) en format Q16.16.
        - Calcule : f_sig = N_counted * FREF_HZ / (end_tick - start_tick).
    
    - **top_reciproc_freq_meas.vhdl** : intègre les deux blocs et gère les signaux de synchronisation ready / meas_done / calc_done.
    - **tb_top_reciproq_freq_meas.vhdl** : banc de test complet comparant :
      - le calcul issu de calc_freq ;
      - le calcul effectué directement par le testbench à partir des ticks mesurés.
    
    - Si la fréquence du signal n’est pas multiple de l’horloge de référence, le comptage des ticks entiers introduit une erreur systématique de ±1 tick.
    - Cette erreur se traduit par une incertitude pouvant atteindre plusieurs dizaines de Hz à 100 kHz (ex. ~98 Hz mesuré pour FREF = 12 MHz, N = 1000).
    - L’erreur ne diminue pas par moyenne sur plusieurs mesures car elle est déterministe, liée au désalignement des fronts entre signal et référence.
    
    - Introduire un bloc d’**interpolation fractionnaire** pour estimer la fraction de tick partiellement écoulée entre les fronts.
    - Objectif : corriger la quantification du temps mesuré (M_eff = M + fraction).
    - Implémentations possibles :
      - TDC (Time-to-Digital Converter) basé sur retard ou sur phase.
      - NCO/PLL numérique pour estimer la phase résiduelle.
    - Résultat attendu : réduction de l’erreur systématique à <1 Hz pour des signaux non synchronisés.

[33mcommit 493647e5b984c9c23e24bffcc2b13046c49eab1a[m[33m ([m[1;31morigin/79-improve-frequencymeter-resolution[m[33m)[m
Merge: 815a097 179050f
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 11:00:25 2025 +0200

    Merge pull request #132 from f4bjh/130-générateur-à-base-daccumulateur
    
    130 - générateur à base d'accumulateur
    
    known limitations :
    - rf_gen is disabled
    - out frequency of NCO is for now limited to 6MHz. Need to increase clk_master

[33mcommit 179050f9cf1d101e814ff8f0d706323afb7df33e[m[33m ([m[1;31morigin/130-générateur-à-base-daccumulateur[m[33m, [m[1;32m130-générateur-à-base-daccumulateur[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 10:48:48 2025 +0200

    130 - [meas/freq] add NCO SPI test driver

[33mcommit e69350827ef895a933b8dcc6a70241c17faa130d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 09:12:55 2025 +0200

    130 - [hw/freq_front_end/reciproq_freq_meas/] command NCO and LED trough SPI

[33mcommit eebcb5480145abb54e246c79ae99a8cfd4f2d0ae[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 07:23:32 2025 +0200

    130 - [hw/freq_front_end/reciproq_freq_meas/] Add NCO integration in top_reciproc_freq_meas with SPI slave
    
    - Added nco.vhd (Numerically Controlled Oscillator)
    - Integrated NCO in top_reciproc_freq_meas.vhd alongside spi_slave
    - Updated testbench to verify both SPI functionality and NCO output
    
    Principle of operation:
    -----------------------
    The NCO (Numerically Controlled Oscillator) implements a simple DDS (Direct Digital Synthesis) core.
    It uses a 32-bit phase accumulator incremented at each rising edge of the input clock.
    
    The increment value (PHASE_INC) corresponds to:
        PHASE_INC = 2^N * f_out / f_clk
    
    where:
        f_out = desired output frequency
        f_clk = input reference clock
        N = number of phase bits (32)
    
    The MSB of the accumulator toggles at the desired frequency, producing a square wave on `clk_out`.
    This method allows precise frequency generation with a digital architecture, without needing analog PLLs.
    
    Example:
    ---------
    With f_clk = 12 MHz and f_out = 100 kHz,
    PHASE_INC = 2^32 * 100k / 12M ≈ 358,400.
    
    The output signal `NCO_OUT` toggles at ~100 kHz.
    
    The top entity now combines:
    - spi_slave: decodes 32-bit SPI words, toggling LED0 on 0x0000FDEC
    - nco: generates a stable frequency clock output
    
    Future work:
    -------------
    The SPI interface could be extended to dynamically set the NCO frequency via received SPI words.

[33mcommit 7fdfa86692448b70e79e78cbf3424584c71e4a72[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Oct 5 07:00:44 2025 +0200

    130 - [hw/freq_front_end/reciproq_freq_meas/] insert  top entity

[33mcommit 66a841a795759d698f6f2bc2085922a0e99df70c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Oct 4 15:08:11 2025 +0200

    130 - reset and cs_n management. Disable rf_gen

[33mcommit 5a889cac58dae59a095b16144e4e289d0ec8883f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Oct 4 11:21:04 2025 +0200

    130 - clean code

[33mcommit 85c3b05fb8d5447573053ff5ccff4bf2b5c3441c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Oct 2 08:13:32 2025 +0200

    130 - change chip select to low active signal

[33mcommit f391bf1d33fb00d7a856a5f828978d1a534b054f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Sep 28 12:52:02 2025 +0200

    130 - add SPI bus driver of FPGA frequencymeter

[33mcommit 61165462c688e5e7a133223c71720d3c3f4ae502[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Sep 26 19:33:16 2025 +0200

    130 - [doc/frequencymeter] add schematic of CmodA7
    
    130 - add pinout type (MRCC, SRCC,...) of FPGA
    
    130 - add CmodA7 reference manual
    
    130 - general .xdc for the CmodA7 rev. B

[33mcommit 03d10da13addac98ace3e80504b466a359c2eca9[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Sep 26 18:50:52 2025 +0200

    130 - create FPGA prj of a reciprocal frequency measurement. 1st draft with a SPI slave

[33mcommit 4c75bcc64927956e97c5e38ec40048ca57108060[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri Sep 26 14:58:00 2025 +0000

    130 - factorization of spi_bus_initialize to prepare use of same SPI bus to the FPGA of frequencymeter

[33mcommit 7e087e7534cf32dae72164f373e13237d2bcc713[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 22 22:40:34 2025 +0200

    130 - remove useless files

[33mcommit 815a0973dd2472de78aac13fbfd441f8f9949322[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 17:03:08 2025 +0200

    79 - time stamping with ChatGPT

[33mcommit 054b9e826908a82905939b9c31354c2b0cf1edb5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 22 21:10:07 2025 +0200

    127 - le compteur de l'ESP32 a une taille max de 32 bits. Du coup, au dela de 4MHz, le compteur overflow. réduit la fenêtre de temps par 4 (precision = 4Hz), et compense dans la chaine de caractère calc_value

[33mcommit 5689570da9f433fb416538edea4a7dc9869d3758[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 22 20:50:49 2025 +0200

    127 - comme on a multiplié par 1000 maintenant, on overflow un int (32 bits). En fait, la fréquence est non signé git add .!

[33mcommit e1807cf5cb080357861640cabc63de7712fb8620[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Sep 21 09:54:05 2025 +0200

    127 - au depart,l'objectif etait d'obtenir une precision de 1mHz, sur le calibre 10MHz. Mais ca suppose des lors d'avoir une fene^tre de 1000*1s...soit 1000s...ce qui n'est pas raisonnable. Alors, a moins d'avoir correctement resolut li'ssue n°79 sur gitlab, qui vise a obtenir et ameliorer la precision pour obtenir cette precision attendue de 1mHz, pour le moment, on met en place un espece de contournement, dans la but d'affichier artificiellement une frequence avec une precision de 1mHz. Par exemple, pour 4MHz, on va afficher 4 000 000, 000 Hz. Les 3 zero aprs la virgule etant ajouté hardcodé ci-dessous. Note : on est oblige de passer un variable tampon "freq", car pdata_cache est un pointeur sur un octet seulement

[33mcommit 421f4f66640dbe4269a061b131f9156f90f60374[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 12:03:11 2025 +0200

    127 - change frequency range display to the up to date frequencymeter archi

[33mcommit 80c7cf4a3845404a28d4fc7e4fc32d59ca971535[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 10:42:24 2025 +0200

    111 - dans le process cadencé par clk_in, on essaye d’affecter une horloge (clk_in) à une sortie (clk_reg) à l’intérieur d’un process synchrone, ce qui fait que la bascule ne génère plus un vrai signal d’horloge. Résultat : clk_out se bloque (souvent à '1' ou '0' suivant la phase au reset). Demandes au flip-flop de recopier l’horloge sur son entrée D, mais cette entrée change exactement au même moment que le front actif → impossible à synthétiser correctement. Solution : laisses le process gérer seulement la division, et fais un mux en sortie

[33mcommit 22a99aa83dc7bf61c5e51e0ed768e675c88b8b71[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 09:55:13 2025 +0200

    111 - gere correctement le cas DIV=1/0

[33mcommit 1b77f5df12ac4b582751a50bcb9f1c7682b3f9e3[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 08:24:40 2025 +0200

    111 - reset_counter_n n'est pas asynchrone

[33mcommit a8fdec5a25b746b3e4b879370902d4b8520ec463[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 08:23:46 2025 +0200

    111 - En VHDL, l'affectation est séquentielle après l'horloge, donc il faut modifier DIV et tu recalculer high_count/low_count dans la même horloge.

[33mcommit 45445fe28628de950a5b56595e744f1f9e37689d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 08:18:54 2025 +0200

    111 - some clean of vivado prject files

[33mcommit 952f286cae8e1e19ec9e38ce6da326939176a4c1[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 20 08:15:39 2025 +0200

    111 - traite le cas DIV=1 à part, car ça veut dire pas de division => clk_out = clk_in. Sinon, high_count-1 overflow

[33mcommit 6899854cdd5dd8237859f1c345db11ebb25a56a2[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Sep 18 00:51:48 2025 +0200

    111 - in a process, it seems that it is really better to assign everything else under rising clock edge. Not very clear at this point, as there is no so big difference between reset_n and reset_counter_n (only, reset_counter_n is an internal signal). Otherwise, it fails to implement saying that [Place 30-574] Poor placement for routing between an IO pin and BUFG. Les BUFG sont faits pour distribuer des clocks globales dans tout le FPGA.
    
    Ils ont des contraintes très strictes : un BUFG doit être alimenté par une source située sur une CCIO (Clock Capable I/O), et en plus dans la même moitié du FPGA (top ou bottom) que le BUFG sélectionné.
    
    Ici, ton signal reset_n arrive sur une entrée normale (IOB_X0Y32), pas sur une CCIO. Vivado tente quand même de le router vers un BUFG (BUFGCTRL_X0Y0), et échoue à cause de la règle de placement.Ne pas passer le reset par un BUFG
    
    Laisse reset_n comme un simple signal logique venant de ton I/O.
    
    Si tu as besoin de le synchroniser proprement, fais-le dans la logique avec un petit circuit de synchronisation à l’horloge

[33mcommit 2589df57993c0f8f404469fd80e4dfe7b52a1781[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 17 23:38:59 2025 +0200

    111 - reset counter when pushing button. we saw that, randomly, generator crashes and does not send any wave at output

[33mcommit c0736668008d9705a3f86ee8ce9dc8a8e7d7d997[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 17 23:17:54 2025 +0200

    111 - 1st draft of new generator, to correct issue of previous counter, wich added extra clock count

[33mcommit 7ad979b4c553fd4bb96f3e60e6267e8a4dfb75a4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 17 23:17:34 2025 +0200

    111 - complete gitignore with big files of vivado

[33mcommit 7d368bfc32e0da60c73514c3772768c6d07e1688[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 17 18:29:44 2025 +0200

    111 - finalize v1 of generator. known issue : divide by 2*MAX + 4

[33mcommit ef92b0ff5836bd7450eea975e259bc8373017257[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 19:15:20 2025 +0200

    111 - add constraint file and complete prj to generate bitstream

[33mcommit e042510d6065cc913de0c4a741b6949dc2b92489[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 17:51:41 2025 +0200

    111 - hf_gen_fpga with testbench. Note : clock wizar is not simulable. it must be implemented in an RTL manner to be simulated

[33mcommit 852530397e1340a5b66592cd3d19f37b31badb25[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 10 23:04:10 2025 +0200

    111 - add a testbench for clock generator project

[33mcommit 9b8aff739672534873451b0c3515940b43052fff[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 10 22:10:21 2025 +0200

    111 - complete led blinker project with a test bench

[33mcommit 884be2bfee4e8c30ec45fc9eb85cab214a67a1cd[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 10 22:00:15 2025 +0200

    111 - implementation of a clock geenrator. pins and constraints have not been set yet

[33mcommit 363410bec967db37dc1eb0c6db43ea4d2de0f91f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 10 21:44:43 2025 +0200

    111 - complete gitignore

[33mcommit 066f08f4aeaee1b26ce0af3dce59f8462c49e57f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Sep 9 23:48:35 2025 +0200

    111 - switch to xc7a35tcpg236-1 FPGA wich is the correct one

[33mcommit 44b3746a5a6624663510c0bea1ea88fc74a29f85[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Sep 9 23:05:51 2025 +0200

    111 - my own led blinker projet

[33mcommit 0a97b7d57204b09b67350d3c78ddf347e65c8064[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Sep 9 21:38:26 2025 +0200

    111 - how-to for beginner on FPGA CmodA7 with GPIO

[33mcommit 7c359d4c80f792b37750ef504748a3a651c82311[m[33m ([m[1;31morigin/124-specification-logicielle-de-lalgorithme-de-mesure-de-la-fréquence[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 12:52:46 2025 +0000

    124 - remove old files

[33mcommit e18ba93e782354cf2e5747e46d239b1e73cd0bc0[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 12:46:49 2025 +0000

    124 - correct file name

[33mcommit 1d4570834f187a06b8c61c3ef82d165321fed29f[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 12:05:26 2025 +0000

    124 - review cosmetic hw/sw doc

[33mcommit ba8a46a56d2741782e3219f5a821ae4b88a3adba[m[33m ([m[1;31morigin/120-a-solution-based-on-mc100ep016a[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 14:38:18 2025 +0200

    120 - update rf hw synoptic to add FPGA Cmod17 board

[33mcommit 128b7cf53834dd4d4d2c6c09eff2969cc7b004d0[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 18:45:12 2025 +0200

    120 - update rf frequencymeter hw synoptic, based on MC100EP016A

[33mcommit 6ea9d632da5abe4a7c207b9b8a24e09a561a6798[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 18:42:25 2025 +0200

    120 - add HMMC3124 doc

[33mcommit 1e73d806f6d266ead4b171b2e3b58ae678a53294[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 18:42:09 2025 +0200

    120 - add PSA2-6+ doc

[33mcommit f899017f827118714da97875169c6a5560ef9ebd[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 18:26:44 2025 +0200

    120 - add SKY13251 doc

[33mcommit 1586ac82e16458315d34b0e802b59d814af12a4a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Sep 13 18:26:26 2025 +0200

    120 - add MAR-6 doc

[33mcommit 37992bff6db588006b3b3506d2ba18f2452c1b48[m[33m ([m[1;31morigin/108-schematic-of-front-end[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 11:45:18 2025 +0000

    108 - move archi doc into relevant folder

[33mcommit 4f161852e6090cfe032afd9c55d5c3cc45d32ffc[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 24 20:56:26 2025 +0200

    108 - correct path to doc

[33mcommit 96a508664151a4374adafea5fbb345a2f03cc2b7[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Sep 9 15:17:33 2025 +0000

    108 - clean doc folder

[33mcommit 361fa1618572f439ad810d92f343de9510bd9006[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Sep 9 00:34:38 2025 +0200

    108 - schematic wip

[33mcommit 6d9fe63dcc1b008587ce7766a3b9e7064e4fe7bd[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 8 23:50:30 2025 +0200

    108 - add ERA2+ kicad library model

[33mcommit d66d46adc4b3a8a86c981449ece7d0d51168db38[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 8 23:36:47 2025 +0200

    108 - add ERA2 amplifier doc

[33mcommit 5ea7e05bc935f6161474a915f296cb39ddc9eec5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Sep 8 19:27:04 2025 +0200

    108 - delete and ignore useless file

[33mcommit 73f9180cad32b72f356db4e19b1185de8ba7ea73[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 21 15:34:16 2025 +0200

    108 - add HMC433E

[33mcommit 29115eb6b1516640ae3b049e22739283d79ed6f9[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 21 14:46:49 2025 +0200

    108 - add rf prescalers datasheet

[33mcommit cd5701859879353b08daaffe150f05d53fc1b55e[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 21 14:32:56 2025 +0200

    108 - update frequencymeter hw architecture

[33mcommit 362c326cc8dfb885b9d11f6e55ed4863896d43ec[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 21 13:54:12 2025 +0200

    108 - add application note of rf prescaler hmc363s8ge

[33mcommit 8975eac8fc3094933531eb87cb5013e611096123[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Apr 28 14:41:20 2025 +0000

    108 - draft of sch

[33mcommit 3a329d0ac1f0c143a1ece14c9275390fda16407c[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Apr 28 16:38:13 2025 +0200

    108 - add datasheet

[33mcommit 46622805429d642a2af633567314d4e51d815e3b[m[33m ([m[1;31morigin/main[m[33m, [m[1;31morigin/HEAD[m[33m, [m[1;31morigin/27-architecture-hw[m[33m)[m
Merge: 90ded19 5140e93
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 13:03:59 2025 +0200

    Merge pull request #133 from f4bjh/89-architecture-mécanique
    
    89 architecture mécanique

[33mcommit 5140e93b7efe45a6ff27fb22608151254172bae3[m[33m ([m[1;31morigin/90-output-of-adf4351-is--5dbm-only[m[33m, [m[1;31morigin/89-architecture-mécanique[m[33m, [m[1;31morigin/119-mise-en-rack[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Sep 25 11:03:24 2025 +0200

    89 - add structure mecanique doc

[33mcommit b060593fc144708f1f39a18a6fa671599e6906ae[m[33m ([m[1;31morigin/134-schematic-of-io-wrapper-board[m[33m, [m[1;32m89-architecture-mécanique[m[33m, [m[1;32m27-architecture-hw[m[33m, [m[1;32m134-schematic-of-io-wrapper-board[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 24 20:48:02 2025 +0200

    89 - push mardown version of doc

[33mcommit 6c27ba79183290c9ee05868c4487b598eab8abf4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 24 20:36:32 2025 +0200

    89 - 1st draft of new hw architecture based on backplane connector

[33mcommit 072efae8c0c164a8f45a1ad559e6200bc228aeef[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Sep 24 20:33:30 2025 +0200

    89 - remove old files

[33mcommit 90ded19fc7a0796ae0d098755f4e980c8576d530[m[33m ([m[1;33mtag: [m[1;33mv1.2[m[33m, [m[1;31morigin/29-start-adc-of-esp32[m[33m, [m[1;32mmain[m[33m)[m
Merge: 74d1c31 f9fb0ef
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sun Apr 27 10:57:09 2025 +0200

    Merge pull request #112 from f4bjh/73-clear-lcd-at-boot
    
    clear lcd at boot

[33mcommit f9fb0ef67b552dd37af3898ff537c0c40ca4111f[m[33m ([m[1;31morigin/73-clear-lcd-at-boot[m[33m, [m[1;32m73-clear-lcd-at-boot[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Apr 18 07:27:53 2025 +0200

    73 - disable clear lcd

[33mcommit 8b8feac1a2687fd467b80cf6e7b3a67d09e7b5b4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Apr 17 19:41:49 2025 +0200

    73 - control the error return value of lcd_init

[33mcommit 56e47ce29b8834ab16fb18265f8732446e49a921[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Apr 17 19:08:15 2025 +0200

    73- clear LCD at very early stage of boot and display IP adress after http server init

[33mcommit b86d1da9e03b7f9f32c988a92adbe4954e741ecd[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Apr 17 18:10:13 2025 +0200

    73 - on factory mode, display only AP mode IP adress, on ota mode, display both IP address (AP and station mode)

[33mcommit 74d1c315170d65a28e68ebd71b8539d0b7cc52b4[m
Merge: 410581a 3df212b
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sat Apr 26 17:00:01 2025 +0200

    Merge pull request #88 from f4bjh/34-start-adf4351-board
    
    Start ADF4351 board

[33mcommit 3df212bd6e8b9e811b8575d4d4aa018eb16ad24e[m[33m ([m[1;31morigin/34-start-adf4351-board[m[33m, [m[1;32m34-start-adf4351-board[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Apr 18 07:34:55 2025 +0200

    34 - parse doc after rebase

[33mcommit 0d068388315dcceeeca6413d80455a2440beb279[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 22:48:40 2025 +0200

    34 - parse doc

[33mcommit bd2ee5019f55125a3defe3c4ef254b400ece7a7e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Apr 13 22:02:13 2025 +0200

    34 - complete fix after rebase

[33mcommit 796f4ebb6b289031d5541b4d0d6a36d43dc0521c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Apr 13 21:23:58 2025 +0200

    34 - compile

[33mcommit a928e8e160035a5ef26e4b56c2444aaef084ad82[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Apr 2 15:57:21 2025 +0200

    34 - use another driver for ADF4351 (one inspired from Analog device)

[33mcommit 27b9f0ec5f4511d2b72c4795cf6434148f279272[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 2 08:53:14 2025 +0200

    34 - fix : set correctly gpio pin on MOSI
    
    due to union in struct spi_bus_config_t, gpio number of MOSI was overwritten.
    Do no initialise anymore data0_io_num

[33mcommit 526850b4acbc0a38b762a4e3f1d167e93b78d397[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 2 07:35:15 2025 +0200

    34 - Fix: Désactivation du DMA pour éviter l'envoi de données nulles sur MOSI
    
    Problème :
    Lors de l'utilisation de `spi_device_transmit()` avec le flag `SPI_TRANS_USE_TXDATA`, les octets à envoyer sur la ligne MOSI étaient tous à `0x00`. Après analyse, il s'avère que ce comportement est dû à l'activation du DMA SPI. Lorsque le DMA est activé, le buffer de transmission est alloué en mémoire DMA, et la copie des données peut être mal gérée si le buffer initial est en mémoire non alignée ou si une mauvaise configuration du cache mémoire interfère avec l'accès aux données.
    
    Solution :
    - Désactivation du DMA SPI pour ce périphérique spécifique afin de forcer l'utilisation d'un buffer non DMA.
    - Ajout de logs de debug pour détecter d'éventuelles incohérences.
    
    Impact :
    - Correction du problème d'envoi de `0x00` sur MOSI.
    - Maintien de la compatibilité avec d'autres périphériques SPI utilisant le DMA.
    - Légère réduction des performances SPI pour les longues transactions, mais garantie d'une transmission correcte des données.
    
    Refs:
    - Documentation officielle ESP-IDF v5.4 sur la gestion du SPI et du DMA.

[33mcommit d8985c58c2a28fdcd7cf5ccd8dd1169685df5922[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Apr 1 00:15:14 2025 +0200

    34 - use SPI3_HOST controller

[33mcommit 235727e236fcbbd05377120526b0b422b2f90bd5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Apr 1 00:14:59 2025 +0200

    34 - modifiy gpio mapping

[33mcommit fd8b38f2bc2ed851f9eda937b6692875f0f08808[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Mar 31 21:47:04 2025 +0200

    34 - initialize all pins

[33mcommit 388b83eb10dea5276ee477e0df15bf8753414bbc[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Mar 31 09:00:22 2025 +0200

    34 - add doc of official eval board of ADF5351

[33mcommit 830a00a7ce44f6d37a02a057e46a3d13a772e11a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 28 09:19:56 2025 +0100

    34 - write hw parameters on device

[33mcommit a3ee5e410b312fee4035e8e61b893209c239df4e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 28 00:45:20 2025 +0100

    34 - manage output value to server of generator html page

[33mcommit 7481a4d899e46e765ab142acd03b0157c24fb298[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Mar 27 22:42:08 2025 +0100

    34 - set output freq to 100MHz

[33mcommit 3975eaa10b2672445ec06d12370f644052a34ff6[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 26 17:12:31 2025 +0100

    34 - update powermeter html mage and js script to display output freq,
    output pwr and generator status (on or off)

[33mcommit bf07174e7e56708e1cbd91e62a6da35d52cc3ecf[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 26 17:11:43 2025 +0100

    34 - add command of usage for the stubed java script in README

[33mcommit 48ac379cb9b0800db6fb87a36167a8ae4bd905ae[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 25 23:41:41 2025 +0100

    34 - update update_rf_gen

[33mcommit efeabb5dbeebb6c93ba8bc1f39618f97243287ae[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 25 23:08:38 2025 +0100

    34 - do nothing in rf_gen_task + preapre update rf_gen paramters

[33mcommit 88ae384d0ff33b093d22152821b01b3a319412e0[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Mar 24 21:27:01 2025 +0100

    34 - parse doc

[33mcommit 1b91759bffa45a52ab5ed5e0a36c90accbd7e900[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 22 14:04:20 2025 +0100

    34 - insert rf_gen measurement module

[33mcommit 410581a7d7580e661e045f6584eb450c9bce1cce[m
Merge: e583f5a 86dfe9c
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 17 09:07:05 2025 +0200

    Merge pull request #110 from f4bjh/80-freq_max30khz-in-frequencymeter
    
    80 freq max30khz in frequencymeter

[33mcommit 86dfe9c3e7b8cb6eb710c1220c8c0b66735786f6[m[33m ([m[1;31morigin/80-freq_max30khz-in-frequencymeter[m[33m, [m[1;32m80-freq_max30khz-in-frequencymeter[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 19:28:28 2025 +0200

    80 - disable and delete pcnt instance properly

[33mcommit 3906de90a35430bffee8f54d4abe1c21bb7b13c0[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 19:20:07 2025 +0200

    80 - frequencemeter: enable PCNT accumulator mode for extended frequency range
    
    Summary:
    --------
    Enable accumulator mode on the PCNT unit to extend the measurable frequency range beyond the 16-bit signed counter limit.
    
    Details:
    --------
    By default, the PCNT hardware counter is limited to a signed 16-bit range (-32768 to +32767), which caps frequency measurements to ~32 kHz over a 1-second sampling period.
    
    This commit enables the `.accum_count = true` option in `pcnt_unit_config_t`, allowing the driver to internally accumulate overflows and return the correct total count directly via `pcnt_unit_get_count()`.
    
    No manual overflow tracking is required. The returned `count` reflects the full number of pulses since the last reset, even if the 16-bit hardware counter has wrapped.
    
    Benefits:
    ---------
    - Accurately measures high-frequency signals beyond 32 kHz
    - No need to handle overflow events or reconstruct pulse totals manually
    - Cleaner and more maintainable implementation
    
    Reference:
    ----------
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html#pcnt-accumulator-mode

[33mcommit e583f5a9f494fdcc02155c847ed9c9a824aa3a3f[m
Merge: 6981e5e 01bcb0f
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 12:05:27 2025 +0200

    Merge pull request #103 from f4bjh/26-architecture-logicielle
    
    26 - update sw doc

[33mcommit 01bcb0f91983c23e9000e2480c05c0c5cb2756f9[m[33m ([m[1;31morigin/26-architecture-logicielle[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 12:04:53 2025 +0200

    Update README.md

[33mcommit 7f24ee6047e09dd728ec3d16aacb6bbb97d4d73c[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 12:04:15 2025 +0200

    26 - update archi sw doc

[33mcommit 6c6909b50e3680e5e9ae6fb485e28fba73c6b66f[m[33m ([m[1;32m26-architecture-logicielle[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 01:14:01 2025 +0200

    26 - archive old doc

[33mcommit 210239ab20a568ad8d6c9ec2e21b3640167a4ede[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 17:14:02 2025 +0200

    26 - Update README.md

[33mcommit d601d6e9c9b4b6da2c812d54347dd9df50a58d48[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 16:24:27 2025 +0200

    26 - add for_developpers help

[33mcommit 100769aa196a6674aaf919802e2f5c841d7f3771[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 16:22:51 2025 +0200

    26 - 1st version of update-fw.md

[33mcommit ad5fcf522dee3267b0059ad16833f487c1c0a5b8[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 08:43:27 2025 +0200

    26 - 1st version of usage doc of wifi and upload menu

[33mcommit fc3b1cd1ff6ea55a2eb4edb9941f99f2f6ef5f74[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 08:21:56 2025 +0200

    26 - update parition table doc

[33mcommit 6981e5ed9725d9e08eb0602103c7506a1eae94cd[m
Merge: deb1132 299f0a4
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 00:16:30 2025 +0200

    Merge pull request #107 from f4bjh/106-in-factory-mode-after-upload-there-is-no-reboot
    
    106 - in factory mode, force reboot just after upload

[33mcommit 299f0a41e9d4b15b739d45b772a370d7490abab4[m[33m ([m[1;31morigin/106-in-factory-mode-after-upload-there-is-no-reboot[m[33m, [m[1;32m106-in-factory-mode-after-upload-there-is-no-reboot[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 16 00:16:01 2025 +0200

    106 - in factory mode, force reboot just after upload

[33mcommit deb11324c171c4b63a5aa1581ffcb2c7d09502c2[m
Merge: 1fa420e c1a03bd
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 22:43:55 2025 +0200

    Merge pull request #105 from f4bjh/27-architecture-hw
    
    27 architecture hw

[33mcommit c1a03bd3b36b992822b2465cae5e93c6bc4e518a[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 15:37:46 2025 +0200

    27 - version 3 of hw synoptic

[33mcommit 55be0c8d0cf862b9f5e0b6d580ad1a9712815d40[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Jan 27 17:06:08 2025 +0100

    27 - version 2 of hw synoptic

[33mcommit 1fa420e762123c9831e4bbe301b9498de8ca1d2e[m
Merge: dfabb82 d7c984d
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 09:01:39 2025 +0200

    Merge pull request #104 from f4bjh/24-rf-frequency-counter-study
    
    24 rf frequency counter study

[33mcommit d7c984d82be78b032f76b0e8b17a1750b2625ae8[m[33m ([m[1;31morigin/24-rf-frequency-counter-study[m[33m, [m[1;32m24-rf-frequency-counter-study[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 09:00:00 2025 +0200

    24 - spec and doc of rf frequencymeter study

[33mcommit d89dd8177f6e5401afa6431c9a05e466bd4cbe57[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Apr 15 07:41:59 2025 +0200

    24 - add esp32-s3 trm

[33mcommit 214495f7a9baef96d2b6ce55f5b59ae90c0c5167[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 22:47:18 2025 +0200

    24 - add doc of SM53MV RF frequencymeter

[33mcommit dfabb82b181ba6ae02d5025547c7779df20bf1b9[m
Merge: f4e337c 66ceb0a
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 08:10:17 2025 +0200

    Merge pull request #85 from f4bjh/26-architecture-logicielle
    
    architecture logicielle

[33mcommit f4e337cb9aab83d2bcdcc8c17572f92ec1d03594[m
Merge: e1d787d ba8d325
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 00:48:32 2025 +0200

    Merge pull request #102 from f4bjh/83-in-upload-menu-let-the-user-to-choose-to-next-partition-to-boot
    
    83 - add checkbox of next partition to boot

[33mcommit ba8d325ff8ae99bee83d2761ab32d3ae4978ab12[m[33m ([m[1;31morigin/83-in-upload-menu-let-the-user-to-choose-to-next-partition-to-boot[m[33m, [m[1;32m83-in-upload-menu-let-the-user-to-choose-to-next-partition-to-boot[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 14 00:47:25 2025 +0200

    83 - add checkbox of next partition to boot

[33mcommit e1d787dfaf788047adc3ace98d7c345f64b37b5b[m
Merge: 08334d1 e47193d
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sun Apr 13 19:22:28 2025 +0200

    Merge pull request #99 from f4bjh/52-factory-partition-shall-have-limited-functionnalities-and-only-ap-mode
    
    52 - add a Makefile for factory and ota fw generation

[33mcommit e47193d859b327a89c661758d56cea5cfa9092d9[m[33m ([m[1;31morigin/52-factory-partition-shall-have-limited-functionnalities-and-only-ap-mode[m[33m, [m[1;32m52-factory-partition-shall-have-limited-functionnalities-and-only-ap-mode[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Apr 13 19:17:16 2025 +0200

    52 - reduce size of factory partition, and increase size of ota partition

[33mcommit 81b8177e6b1be1b41e992c42f12eed57740f1683[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Apr 11 08:11:50 2025 +0200

    52 - enable both mode (AP and station) on ota partition

[33mcommit 4c1263f9fac9847503cf380bd61d1722fd8fa380[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Apr 10 08:11:55 2025 +0200

    52 - update CMakeLists.txt to compile only needed upn factory or ota partition flag

[33mcommit da0a9c5e4bac35264717f3f5ff34a78132646708[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 9 08:24:40 2025 +0200

    52 - on factory partition, serve only 1 html page, wich is only the upload page in fact

[33mcommit a1992c1c8114177729ef37e4b5937052e2c7b6a7[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Apr 9 07:56:16 2025 +0200

    52 - fix : delete old script.js from html pages

[33mcommit ad7e5f3524dfbcb68e7961d8b6688c3b42cb4189[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 7 23:56:34 2025 +0200

    52 - add a Makefile for factory and ota fw generation

[33mcommit 08334d11e5112e32abf3d40ac8e46b03e3b35dca[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 7 22:27:59 2025 +0200

    not used

[33mcommit d540fbbcf111e9117a58a5a58354b30aafcdb763[m
Merge: b189168 59ed1fb
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Apr 7 22:19:22 2025 +0200

    Merge pull request #98 from f4bjh/97-failed-to-connect-to-websocket-server
    
    97 failed to connect to websocket server

[33mcommit 59ed1fbf0ec09f399afa031252e06b7f89010c70[m[33m ([m[1;31morigin/97-failed-to-connect-to-websocket-server[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 7 22:18:14 2025 +0200

    97 - clean

[33mcommit 713e23067cadb5b4e15fd7e15d6b410e4290aa1d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Apr 7 22:05:54 2025 +0200

    97 - fix: centralisation de l'initialisation WebSocket dans un seul fichier
    
    Description :
    Refactorisation du code client pour centraliser l'initialisation de la connexion WebSocket dans chaque script JavaScript. Tous les gestionnaires d'événements liés à la WebSocket (onopen, onmessage, etc.) sont désormais définis au même endroit, garantissant une exécution cohérente et fiable.
    
    Problème résolu :
    Sur les serveurs Web embarqués ESP32-S3 (souvent via esp_http_server ou équivalent Arduino), des problèmes peuvent apparaître lorsque la logique WebSocket est répartie dans plusieurs fichiers .js :
    
    L’ordre de chargement et d’exécution des scripts n’est pas toujours garanti par les navigateurs, surtout si les scripts sont asynchrones ou si le serveur est lent à répondre.
    Le chargement parallèle de plusieurs fichiers JS peut dépasser les capacités limitées du serveur web de l’ESP32-S3, entraînant des réponses incomplètes ou retardées.
    Il en résulte souvent que certains scripts tentent d'utiliser window.socket avant même qu’il ne soit initialisé, ou que les événements ne sont jamais correctement attachés.
    Solution mise en œuvre :
    Regroupement de la logique d’initialisation et de gestion des messages WebSocket dans chaque script.
    Les autres modules JS peuvent désormais accéder à une socket déjà initialisée et fonctionnelle via socket, sans risque de condition de course.
    
    Impact :
    Améliore la robustesse du client web embarqué.
    Évite des comportements erratiques sur des navigateurs modernes comme Brave ou Chrome.
    Facilite la maintenance du code en centralisant les responsabilités.

[33mcommit cf56cb4005a5067f35cda71e78d3e874de1d8890[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Apr 5 08:18:02 2025 +0200

    97 - add some trace in case of web server init error

[33mcommit b1891687c4930e73592cac5acbe4eaa64f448005[m
Merge: fc59c49 fe244f9
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri Apr 4 13:47:11 2025 +0200

    Merge pull request #96 from f4bjh/69-when-uploading-a-new-firmware-is-successfull-popup-a-window-of-the-status-that-invites-the-user-to-reboot
    
    69 - instead of poping a window, force the user to reload index page

[33mcommit fe244f93b3a16d4ef914ceb6598915896efe51e1[m[33m ([m[1;31morigin/69-when-uploading-a-new-firmware-is-successfull-popup-a-window-of-the-status-that-invites-the-user-to-reboot[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Apr 4 13:46:31 2025 +0200

    69 - instead of poping a window, force the user to reload index page

[33mcommit fc59c49167c43d4ffa8bd2b55b1b042cf3c9e31d[m
Merge: 01184a4 50c9fdf
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri Apr 4 13:03:08 2025 +0200

    Merge pull request #87 from f4bjh/84-review-src-architecture-fo-web-server
    
    review src architecture fo web server

[33mcommit 50c9fdf96bbeee295666335701ef68e826395567[m[33m ([m[1;32m84-review-src-architecture-fo-web-server[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Apr 4 12:46:39 2025 +0200

    84 - add trace log in main

[33mcommit 9aec1b371b3448857f43fb4d6bd7e786aed49327[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 12:18:31 2025 +0000

    84 - handle new js file in web server

[33mcommit 92023ad00055ff19dd7e7d8b23545a727f60b011[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 12:18:09 2025 +0000

    84 - add new created file to build

[33mcommit f8ec157a5a820f19f69cff276be53e6d308044f9[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 12:17:51 2025 +0000

    84 - split javascript script.js per page

[33mcommit 0251fb7652386ed277835cc72acfad3cfa234734[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 11:19:43 2025 +0000

    84 - clean

[33mcommit a415736dc7e18ca79d9cc9eeeb0f1a4f969940e9[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 10:04:18 2025 +0000

    84 - split powermeter from server.c

[33mcommit def645bb1e558d413f18b51198504b0599df2031[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 10:00:52 2025 +0000

    84 - split generator from server.c

[33mcommit bc698b8cfe7f10c0e60d1ac47b2d0f93bdd60efa[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 09:46:31 2025 +0000

    84 - split index from server.c

[33mcommit c9f0f82d934905603c357029e669f77ba4228009[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 09:02:20 2025 +0000

    84 - split ws from server.c

[33mcommit 6370c98ef6b3a61a39b1bc9825d9974d09f638e6[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 08:48:58 2025 +0000

    84 - clean

[33mcommit 6782730de44e1e9b6d50d1907bca2e1fba5051ca[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 08:35:18 2025 +0000

    84 - split upload from server.c

[33mcommit 3dfc327d9cf2268ee27a539a6356609a05709582[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 08:13:25 2025 +0000

    84 - split frequencymeter from server.c

[33mcommit 503261f1b8f5db83581b159559ce40be39813536[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Apr 3 07:25:46 2025 +0000

    84 - split wifi from server.c

[33mcommit b1836a5c1b31c01ee6cbb87fa8264b93171b55e4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 26 08:49:08 2025 +0100

    84 - place websocket send data into a specific task, so we are not stuck anymore into ws_server_send_date. return of http_server_init is reached

[33mcommit bb8d0f0f03f8154c4edc910d129d10b72ce3a3ff[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 26 07:43:19 2025 +0100

    84 - split about from server.c

[33mcommit 9f27ba2e76aa7732e99ebc1d250bcc7cd093e1fa[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 26 07:42:17 2025 +0100

    84 - rationalize include

[33mcommit 01184a4d52fb53114001aacaf2737910a3db9c3e[m
Merge: caa3a41 a730e27
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sat Mar 22 14:27:16 2025 +0100

    Merge pull request #86 from f4bjh/78-it-fails-to-boot-if-there-is-no-lcd-connected
    
    78 it fails to boot if there is no lcd connected

[33mcommit a730e277d0f1bceb86aff4bc2f58bc5d5ec5b100[m[33m ([m[1;31morigin/78-it-fails-to-boot-if-there-is-no-lcd-connected[m[33m, [m[1;32m78-it-fails-to-boot-if-there-is-no-lcd-connected[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 22 14:26:39 2025 +0100

    78 - do not block system init, even if there is no lcd materialy connected

[33mcommit df9c359b6a728f9abbbb08017c194be6a78bbe69[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 21 08:56:32 2025 +0100

    34 - add gpio mapping file

[33mcommit caa3a415de2a4b51e6528bd7ae09418d23017ea4[m[33m ([m[1;33mtag: [m[1;33mv1.1[m[33m)[m
Merge: 92b4e33 a63e8bc
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 23:35:37 2025 +0100

    Merge pull request #76 from f4bjh/28-esp32-frequency-counter
    
    28 esp32 frequency counter

[33mcommit a63e8bc551aec249c28f40c422d6121a6107647c[m[33m ([m[1;31morigin/28-esp32-frequency-counter[m[33m, [m[1;32m28-esp32-frequency-counter[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 15:22:31 2025 +0100

    28 - add freq_range selector in html, and manage the placement of the
    coma

[33mcommit 1fa7d1ea4c3ddd8a2b58cfd741ab400d95b8f8b1[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 16:10:33 2025 +0100

    28 - add gptimer_disable
    
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/gptimer.html

[33mcommit 1b2f94d4409f3be0540d949256d19cb203a89ad9[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 19 22:40:24 2025 +0100

    28 - destubed frequency measure and reports correctly (byte by yte) frequency value)

[33mcommit a6cf73699fb7812003d7631826fba7766e3a5b8e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 19 21:36:45 2025 +0100

    28 - complete commentary for resolution set

[33mcommit 92c0548b9995516646079e42d209b47a8aec0252[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 19 14:58:39 2025 +0100

    28 - Add some doc

[33mcommit ab9924b2ba88c1a957467062502e7391b8ea7ec9[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 23:35:10 2025 +0100

    28 - not needed ?

[33mcommit 3f0cd6452353facedf80d921f22d199039f77e99[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 20:19:39 2025 +0100

    28 - set correctly measure size in part_info

[33mcommit 71e6d7ec6a62b353d5c7fb4ec353bc86f7c27456[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 07:06:16 2025 +0100

    28 - generalize handle to be released in remove state

[33mcommit 9ef0988cff68808d2ec488bda34bf9c490c6387a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 06:13:01 2025 +0100

    28 - insert frequencymeter value in html page

[33mcommit 23c5121786d8ce65bcdb035233f9c56110eb7b8d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 05:29:56 2025 +0100

    28 - add frequencymeter

[33mcommit b377aaeb043c86eaf07df03415e62d27d1bacaee[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 18 05:29:32 2025 +0100

    28 - add doc of pinout of the board

[33mcommit 92b4e33fbe380d13bb445c4cd6381205d82f9e6c[m
Merge: 8fc197c 1b6761c
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 19:34:22 2025 +0100

    Merge pull request #72 from f4bjh/68-add-reboot-button-in-wifi-page
    
    68 add reboot button in wifi page

[33mcommit 1b6761c79035cb2c44d112b94467710b0f6e4b99[m[33m ([m[1;31morigin/68-add-reboot-button-in-wifi-page[m[33m, [m[1;32m68-add-reboot-button-in-wifi-page[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 16:48:01 2025 +0100

    68 - add a reboot button in wifi page

[33mcommit 1be3f16078e0aa58f701a826052ad7686ea1632d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 16:44:57 2025 +0100

    68 - current part version is incomplete

[33mcommit 66ceb0a6457aa15d7d06fe883db03bea75003620[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Mar 17 14:37:53 2025 +0100

    26 - add FSM measurement in doc

[33mcommit 8fc197c84e081219df2b11aa089583f5a08f2494[m[33m ([m[1;31morigin/36-start-pe4302-board[m[33m)[m
Merge: 52cee0c 8181d95
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 17:04:23 2025 +0100

    Merge pull request #74 from f4bjh/64-handle-both-homepage-indexhtml-and
    
    64 - add handle uri /

[33mcommit 52cee0c61f6383877060253a8880f8b226e96002[m
Merge: b5f1fb1 7ea66f7
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 17:01:34 2025 +0100

    Merge pull request #71 from f4bjh/10-execute-http-server-on-a-single-cpu
    
    10 - pin all known tasks to CPU0

[33mcommit b5f1fb1a03e09787b1f080cc5a773a530f616fe3[m
Merge: 8e3b78d 994e543
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 20 17:01:19 2025 +0100

    Merge pull request #70 from f4bjh/66-do-not-print-wifi-credentials-in-console
    
    66 - increase debug level of wifi credential display in console. It l…

[33mcommit 8181d9568b7e1a5becf6f280b479f1c78085acc7[m[33m ([m[1;31morigin/64-handle-both-homepage-indexhtml-and[m[33m, [m[1;32m64-handle-both-homepage-indexhtml-and[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 23:07:39 2025 +0100

    64 - add handle uri /

[33mcommit 994e543c15c5e972041b647c9d59ecabd47c4864[m[33m ([m[1;31morigin/66-do-not-print-wifi-credentials-in-console[m[33m, [m[1;32m66-do-not-print-wifi-credentials-in-console[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 16:28:11 2025 +0100

    66 - increase debug level of wifi credential display in console. It let developpers use this dbg msg in cas if need

[33mcommit 7ea66f70994a41c6d167b72459540bf18ea80f3c[m[33m ([m[1;31morigin/10-execute-http-server-on-a-single-cpu[m[33m, [m[1;32m10-execute-http-server-on-a-single-cpu[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 16:12:06 2025 +0100

    10 - pin all known tasks to CPU0

[33mcommit 8e3b78d4ad2f0ab895a7c5696267d3048c7b37fb[m
Merge: 9624494 3825b98
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 16:08:42 2025 +0100

    Merge pull request #67 from f4bjh/65-cpu-counter-set-to-unknown-in-ap-wifi-mode
    
    65 - even on factory partition, with no ota_0 or ota_1 when flas has …

[33mcommit 3825b98bdafb64091107301928525b495997de7e[m[33m ([m[1;31morigin/65-cpu-counter-set-to-unknown-in-ap-wifi-mode[m[33m, [m[1;32m65-cpu-counter-set-to-unknown-in-ap-wifi-mode[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 14:03:48 2025 +0100

    65 - even on factory partition, with no ota_0 or ota_1 when flas has been totally erased, do not block fsm measurment on CPU0 and CPU1 counter task. There should not have any link beetween counter task (CPU0 or CPU1), and next partition value, but for some (unknown) reason for now,, empty ota_0 or ota_1, leads to a failing returned value of counter. weird so far...

[33mcommit 962449466a4f34d0fb80d950d046ac310a37488d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Mar 16 10:14:05 2025 +0100

    add some doc and datasheet

[33mcommit 45651e49d25039de787d96918a6368e1f432e5f8[m
Merge: 5026c48 e36ac14
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sat Mar 15 10:34:06 2025 +0100

    Merge pull request #59 from f4bjh/53-review-get-measurment-task
    
    53 review get measurment task

[33mcommit e36ac148e9ccfa1a8b136018fb8f67b2458c5d43[m[33m ([m[1;31morigin/53-review-get-measurment-task[m[33m, [m[1;32m53-review-get-measurment-task[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 15 09:00:29 2025 +0100

    53 - create a counter on both CPU core. Insert input measurement parameters

[33mcommit 1efb2cd377555c0b2250aa394085886aefc166d5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 15 07:57:14 2025 +0100

    53 - use snprintf instead of sprintf

[33mcommit c762829bed2827793c9fe6382816372355ff0cd3[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 19:40:59 2025 +0100

    53 - review json process parsing sent by client

[33mcommit c334992cf983d96c3b9e5f2a3ffda7348c32686d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 15:54:19 2025 +0100

    53 - complete index.html measurement

[33mcommit 924f67d4123d4f78bcc67b99493eaa22092b7631[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 15:47:10 2025 +0100

    53 - add current partition measure

[33mcommit 8e79db9174574520f8ad075fab4e3f85cebde796[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 14:37:58 2025 +0100

    53 - review meas architecture src

[33mcommit 39cbe58ea2a5919df2dbcb736b20dc28723068a0[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 13:55:06 2025 +0100

    53 - improve counter

[33mcommit eee3366ea23d8417fb0088fcc7ad0be2a82418f1[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 11:22:16 2025 +0100

    53 - add a template of measurement. based on counter functionnality. template helper for compilation
    
    53 - implement a counter measurment on CPU1 exclusively

[33mcommit a99e1d99efbaeb26ea8fe0014e63634dfdef3e84[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 10:28:09 2025 +0100

    53 - Fix memory corruption by declaring meas_fsm_task_arg as static
    
    The meas_fsm_task_arg structure was previously declared as a local variable
    in meas_mgt_init(), leading to memory corruption issues.
    
    Problem:
    - meas_fsm_task_arg was allocated on the stack of meas_mgt_init().
    - After meas_mgt_init() returned, its stack was released, making
      meas_fsm_task_arg invalid.
    - When meas_fsm_task accessed meas_fsm_task_arg, the data was corrupted
      (e.g., instance_meas truncated).
    - This could cause undefined behavior, invalid memory access
      (LoadProhibited), and crashes.
    
    Solution:
    - Declare meas_fsm_task_arg as static so that it is stored in global memory
      instead of the stack.
    - This ensures that the structure remains valid even after meas_mgt_init()
      completes.
    - Prevents memory corruption and invalid accesses in meas_fsm_task().
    
    Impact:
    - Fixes crash (Guru Meditation Error: Core panic'ed (LoadProhibited)).
    - Ensures data integrity when passing arguments to meas_fsm_task().
    - Improves program stability by preventing invalid memory access.

[33mcommit bd2459fdffbf5f50830f1512d36ef6c0a8618e8c[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 13 15:21:03 2025 +0100

    53 - initialise intance_meas per html page

[33mcommit dc087fa447892027019f82a83ebf98ec325c1cd4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Mar 13 00:59:20 2025 +0100

    53 - send page Id to the server through in json trough ws

[33mcommit 5995bd88cb5697d91a55d8b7a7b26f53bfd3629d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 23:36:01 2025 +0100

    53 - response TEXT with no payload

[33mcommit 89536723f6c514dab00ac277230d8c3ae0a1e851[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Mar 11 00:55:34 2025 +0100

    53 - use user_ctx field of server to forward page loaded by the client
    
    53 - use wss_open and wss_close to initialise and remove instance_meas
    
    53 - use input value to be printed
    
    53 - sanity check before free memory
    
    53 - move init and remove instance_meas in ws_handler

[33mcommit 349c4e68b8fdd6f481a920f8f9826ab2f5d1e43d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Mar 8 17:40:12 2025 +0100

    53 - insert meas_func in meas_t

[33mcommit 1fb10b4365b6476d5d9a4a7e0a20a4d079a70f39[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Mar 3 23:51:55 2025 +0100

    53 - add call of get_chip_info_model in server on CPU0, to get data as an example for now only

[33mcommit af7f8ac11e63ebaa911d735737e81f23f26cf620[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Feb 26 08:27:35 2025 +0100

    53 - introduce fsm for get measurment task
    
    53 - initial state of measurment is MEAS_INIT, to enable the fsm
    
    53 - handle an action, only if there is an action in the queue
    
    53 - add a time of sleep for fsm taks, else CPU does not go into IDLE, wdt is triggered
    
    53 - correct dbg msg
    
    53 - initialize ready boolean
    
    53 - init state is pending and add call to call back to run fsm
    
    53 - add debug uart msg in each state
    
    53 - do not remove measure if it has not been sent less than once
    
    53 - do not reset ready flag once in remove state. have to be managed by http server

[33mcommit 3dd3c5a524454ae11fd10818e363ee0cdbe644c5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Mar 14 13:25:42 2025 +0100

    53 - update README with some assembly analysis commands

[33mcommit 5026c489dc44c56fb285261b64d10fcc3099c6a5[m
Merge: acdcd17 eb2c3ec
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 13 02:02:05 2025 +0100

    Merge pull request #61 from f4bjh/58-add-qemu-in-ci-for-tests-purpose
    
    58 add qemu in ci for tests purpose

[33mcommit acdcd17555d4664cc9e625a4bedbed5957d9a273[m
Merge: 565eebe 54615e8
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 13 02:01:32 2025 +0100

    Merge pull request #60 from f4bjh/55-client-disconnect-after-1st-measure
    
    55 client disconnect after 1st measure

[33mcommit 54615e853d3252bd0541bcc41f795210379ac17c[m[33m ([m[1;31morigin/55-client-disconnect-after-1st-measure[m[33m, [m[1;32m55-client-disconnect-after-1st-measure[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Feb 21 08:54:26 2025 +0100

    55 - improve debug msg on client on connection + initiliaze variable

[33mcommit 565eebe46546ba8e4735e673c51188e3e34cd8e1[m
Merge: d2f064e 9625b5e
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Mar 13 01:59:14 2025 +0100

    Merge pull request #57 from f4bjh/54-ip-address-on-lcd-is-not-correct
    
    54 ip address on lcd is not correct

[33mcommit 9625b5eb5f9def8a0b74458cc54eed7bab41aaf1[m[33m ([m[1;31morigin/54-ip-address-on-lcd-is-not-correct[m[33m, [m[1;32m54-ip-address-on-lcd-is-not-correct[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri Feb 21 11:51:18 2025 +0100

    54 - insert mode bit value AP or STA

[33mcommit eb2c3ec7323e5daab2efe1aee33d68ca0f535a19[m[33m ([m[1;31morigin/58-add-qemu-in-ci-for-tests-purpose[m[33m, [m[1;32m58-add-qemu-in-ci-for-tests-purpose[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 18:31:07 2025 +0100

    58 - Install SDL2 dependencies

[33mcommit b51f652a9223f5d313150da0809ff89ff07feec6[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 18:17:52 2025 +0100

    58 - add Install qemu-xtensa

[33mcommit 10ba9603ae439227d0ce3c01f5b48c07f16fc483[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 17:50:40 2025 +0100

    58 - Recharge l'environnement ESP-IDF

[33mcommit 65255709f69886f7d0251db143682045404c8d5c[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 17:39:16 2025 +0100

    58 -  clone ESP-IDF depuis le dépôt officiel

[33mcommit 5e9d0b3f1759e64bfee3b253631d41ad47e8ed9c[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Mar 12 17:14:57 2025 +0100

    58 - use idf.py for build and qemu

[33mcommit aa64e83f6336b9bc3d93a5054bcffba0c19d1ea7[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Fri Feb 21 08:00:27 2025 +0100

    55 - let more time to the client to send his PONG reply

[33mcommit d2f064e2867387947e52b08907fd9b2eb5b77d7d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 23:01:18 2025 +0100

    lack of debug msg

[33mcommit fab8d1941cb0a439eda4b84b7d51f8a157e07740[m
Merge: 6be6714 7aa815b
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 22:54:26 2025 +0100

    Merge pull request #51 from f4bjh/40-switch-from-wifi-access-point-into-a-wifi-standalone-client
    
    40 - implement wifi credential of AP mode

[33mcommit 7aa815b082ee24411ede722bf7483d9d6abd1443[m[33m ([m[1;31morigin/40-switch-from-wifi-access-point-into-a-wifi-standalone-client[m[33m, [m[1;32m40-switch-from-wifi-access-point-into-a-wifi-standalone-client[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 22:51:12 2025 +0100

    40 - enable wifi credentials

[33mcommit 9ed9cc74c19f6a699df812780307b87e0627baa5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 08:45:37 2025 +0100

    40 - parameter flash key name are too long

[33mcommit c27064eff06b480df7d91bdfd3a5932ba0296a38[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 08:30:14 2025 +0100

    40 - add wifi_set_credential parameter in flash

[33mcommit bee29b7073179902aa3f845984f1f662088bf1f1[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 20 07:53:41 2025 +0100

    40 - retrieve correctly the number of bytes received in POST request

[33mcommit f0913c7db2d60f35588a054286e5a84e677f1ff4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 17 22:37:37 2025 +0100

    40 - display IP address of STA mode only on lcd for now

[33mcommit d30696b9160209fd10622c2839abdf9f22825cc7[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 17 22:21:59 2025 +0100

    40 - not used anymore

[33mcommit 7d0932548405620953d8533369f422e240393765[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 17 22:15:40 2025 +0100

    40 - get the IP address of ws socket server in js script

[33mcommit 9dc2fa4a5b195d341feb011daae315d932baff58[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 17 20:28:12 2025 +0100

    40 - set hostname to Rf-Test-BJH

[33mcommit 6eb7681762c620510e5fea0846ec3a50676877b6[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Feb 16 16:10:43 2025 +0100

    40 - AP and STATION mode on separate build

[33mcommit ff438620038e65d255874427a914f5a3f3a0111c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Feb 16 15:46:05 2025 +0100

    40 - update gitignore

[33mcommit f99eb0c760e7cf4bbe85dc4ff73e9d631f48e791[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Feb 12 23:17:23 2025 +0100

    40 - enable sta mode

[33mcommit ae2d2b30b0df1615d45b01354c8c88f9cda43ed3[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Feb 12 23:04:48 2025 +0100

    40 - implement wifi credential of AP mode

[33mcommit 9ea9573a3d3a6aa5fde204b5059a94b2c38f3af5[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Feb 12 22:43:26 2025 +0100

    40 - reimplement wifi AP mode

[33mcommit 6be671462b703a6f2df59345c0c6d17d1d80e86e[m
Merge: 6c1a06e 246ce59
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Mon Feb 10 23:19:24 2025 +0100

    Merge pull request #50 from f4bjh/16-application-crashes-randomly
    
    16 application crashes randomly

[33mcommit 246ce59af1e86571132cf80de19248ec51effe84[m[33m ([m[1;31morigin/16-application-crashes-randomly[m[33m, [m[1;32m16-application-crashes-randomly[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 10 23:00:20 2025 +0100

    16 - supsend keep_alive task when downloading a new fw

[33mcommit bf558bc2049c2be80c7cb71d00033bc01e2c15d9[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 10 22:52:35 2025 +0100

    16 - update keep alive after download, not needed anymore

[33mcommit b0d7f473390b7c12ba08c8673cdf76cd2f6c589d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 10 22:40:02 2025 +0100

    16 - close as soon as possible all fd that are not ws socket client

[33mcommit 7e4997925c5838f82f9add3a5fa561615fd1f431[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 10 21:29:16 2025 +0100

    16 - improve debug msg

[33mcommit 3b0751294032531a9ae52b70828c1ed4accb4598[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Feb 9 10:43:22 2025 +0100

    16 - change priority of task

[33mcommit 16e5a5a859b9c10124c8415ab9c0dfac7c655a58[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Feb 9 10:43:00 2025 +0100

    16 - improve debug msg

[33mcommit 84bca4a68a2cd2eee44ed06f197776c4d995f51e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 19:07:44 2025 +0100

    16 - if, for some reasons, there is no data to send to client, do not even try to send them

[33mcommit ffb9df26721bbe3b080bd6cf732f006b27347477[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 18:47:16 2025 +0100

    16 - increase nb of measurement in queue

[33mcommit 50b178b8d76971015d1921ccab2220c004f4cec8[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 18:32:40 2025 +0100

    16 - add esp tools command to erase flash in README

[33mcommit 1f1610bac07f905b08f8378098ba3eb702f52e04[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 18:05:38 2025 +0100

    16 - increase stack size of keep_alive task

[33mcommit 6c1a06eb875f2c72db571aa99dffd1eb4c39d2eb[m[33m ([m[1;33mtag: [m[1;33mv1.0[m[33m)[m
Merge: 516f1a1 98f587f
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 16:03:22 2025 +0100

    Merge pull request #46 from f4bjh/21-display-ip-address-on-lcd
    
    21 display ip address on lcd

[33mcommit 98f587f5851209bffc8d45938d3c94193f314220[m[33m ([m[1;31morigin/21-display-ip-address-on-lcd[m[33m, [m[1;32m21-display-ip-address-on-lcd[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 16:00:17 2025 +0100

    21 - update sdconfig.default

[33mcommit 60330f541bc2dcf4d324fa8ec3300c00ed6a8358[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 8 15:55:20 2025 +0100

    21 - display application info and IP address

[33mcommit a72e39fc16d36ee194c0d269be9a724f0f661ffa[m
Merge: 6a5fc4f e0dde46
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 23:22:26 2025 +0100

    Merge remote-tracking branch 'origin/26-architecture-logicielle' into 21-display-ip-address-on-lcd

[33mcommit e0dde462c1a9f2eb6a5487a4a8d93d7834af91e8[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 23:19:17 2025 +0100

    26 - partitions needs to be align with 64kB (for a security reason somehow : https://github.com/espressif/esp-idf/commit/57b601ab7f6254e98b29d6f48124055b59f57d15)

[33mcommit bbbef9272dbf7ed670213167c6b9b3833c33654e[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 02:21:13 2025 +0100

    21 - improve flash size to 16MB (my board is ESP32-S3-DevKitC-1-N16R8)

[33mcommit c55fe1aa96ed3bbf8b79fb01f1b069f9e569fbd6[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 20:12:41 2025 +0100

    26 - define partition table

[33mcommit 2fad45199be2925e1ce3c42c87c2d500d338fb40[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 14:31:49 2025 +0100

    26 - Add partition table of flash

[33mcommit 516f1a1eb30f30767239081ac47a508ac2fc5ccd[m
Merge: af33227 611d370
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 09:17:09 2025 +0100

    Merge pull request #48 from f4bjh/42-add-idfpy-fullclean-in-readme
    
    Update README.md

[33mcommit 611d37057020c3b93bd26c45daa6366beb7f8b19[m[33m ([m[1;31morigin/42-add-idfpy-fullclean-in-readme[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 09:16:39 2025 +0100

    Update README.md

[33mcommit 6a5fc4f97df6a3614bb91050758c1a3f5c9fbcc4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 02:21:13 2025 +0100

    21 - improve flash size to 16MB (my board is ESP32-S3-DevKitC-1-N16R8)

[33mcommit 96abae5484a1c1a6e24fad2ae97c63972036bf56[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 02:20:01 2025 +0100

    21 - use lcd in app_main

[33mcommit d44e9a243ca6f237c564f390eedec5df0a440497[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:51:35 2025 +0100

    21 - update gitignore

[33mcommit 54dda13c9d08b0f7d47e7df42c661369800d2bdf[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:51:19 2025 +0100

    21 - update sdkconfig to include lcd and his dependencies

[33mcommit 02722db6b5554f8f403752aab153c3d4fbe6ad15[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:46:55 2025 +0100

    27 - add dependencies for lcd and lvgl

[33mcommit 9ce0399eb763f844fd6aeae29c54e513c48d559a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:46:18 2025 +0100

    21 - add lcd driver and user application

[33mcommit 4ca4df70b180fa1d1a5787f0b58d591befc5ae95[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 00:21:07 2025 +0100

    21 - add a saved sdkconfig as a remainder

[33mcommit af33227b6771ef09d3088e1fa4d49667b7cbb545[m
Merge: 14463e2 6733974
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:30:22 2025 +0100

    Merge pull request #45 from f4bjh/43-build-with-v54-esp-idf
    
    43 build with v54 esp idf

[33mcommit 6733974d3cc00f081e7956e691991a012d5379f2[m[33m ([m[1;31morigin/43-build-with-v54-esp-idf[m[33m, [m[1;32m43-build-with-v54-esp-idf[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:17:49 2025 +0100

    43 - update CI to esp-idf 5.4

[33mcommit 58297746f3a2fc3496a486068bd02c910cd45a65[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:17:28 2025 +0100

    43 - update sdkconfig to esp-idf 5.4

[33mcommit 4931c79a9b60cee0a8a3574c45a38549bf724011[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Feb 6 01:17:04 2025 +0100

    43 - update README with how to update esp idf environnement

[33mcommit 14463e2cd696466c031f378ce23de53cadfe04c7[m[33m ([m[1;31morigin/39-reduce-verbosity-of-console[m[33m)[m
Merge: e25fbb3 c51de61
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Feb 5 00:38:40 2025 +0100

    Merge pull request #33 from f4bjh/7-partition-update-switch
    
    7 partition update switch

[33mcommit c51de61742939d5151170a7c880a9f31f32b6ea7[m[33m ([m[1;31morigin/7-partition-update-switch[m[33m, [m[1;32m7-partition-update-switch[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Feb 5 00:32:31 2025 +0100

    7 - add info of current and next partition

[33mcommit bebafb3fa130e5e1ff597d32151c9d22c06e77b4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 23:36:21 2025 +0100

    7 - partition info in both index and upload page

[33mcommit f0cd9b3040b59584bd7bc4bd11ab835e03cf2092[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 23:25:44 2025 +0100

    7 - set next boot partition only when user click on reboot

[33mcommit 90c94b3bbab626bdde015e612558b91d640388a3[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 22:23:03 2025 +0100

    7 - display current and next partition in upload page

[33mcommit cc7094081af5e647c238f42ad198204f8d873485[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 21:28:22 2025 +0100

    7 - pageId contains the extension of the file name of html file

[33mcommit 912ff89d87c4080db049290102f67b4237b7766d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 21:27:53 2025 +0100

    7 - restart from beginning in get_measurement task

[33mcommit 746cad670efaaacf9194e4c3cf294682d33f6773[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 21:21:19 2025 +0100

    7 - call script.js and define message-container space in html

[33mcommit b8549ebcfb72dcd46e17995022de7706435e8997[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 3 19:02:27 2025 +0100

    7 - add current partition value in html pages and in measurment task manager and in server task

[33mcommit cc8167535240b590ed6f088b5d76c09fe7272b0d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 1 17:35:45 2025 +0100

    7 - reformat upload.html page

[33mcommit fd57e18fa998d0d5446d3341b427b60d1ac0b4d7[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 1 15:32:01 2025 +0100

    7 - add reboot button

[33mcommit cadc37866c54c708af814fa16048336c52c0cc2f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 1 09:55:36 2025 +0100

    7 - prepare html pages

[33mcommit 439f75fabd1de97f2fb5a0345b8279ca41cd1b3a[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 1 13:42:51 2025 +0100

    7 - add partition description of running partition into console log

[33mcommit 1e97bd2146d83b06492b44db7b8a99e84eadbb61[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat Feb 1 09:04:29 2025 +0100

    7 - print current partition in console before all initialization

[33mcommit 2a21fc45ebe7cbfaa820aa855acde8941bade306[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 02:20:20 2025 +0100

    7 - delete dead code

[33mcommit e25fbb37777b4a5381cfb748e174e50945502bf3[m
Merge: 9d0910d c864143
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 20:37:10 2025 +0100

    Merge pull request #32 from f4bjh/17-if-websocket-client-is-placed-in-a-javascript-file-it-fail-to-open-connection
    
    17 if websocket client is placed in a javascript file it fail to open connection

[33mcommit 9d0910de7152575b5f8fce8017ebe3ef15bc1ebb[m
Merge: 89e7596 fea49e0
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 20:30:03 2025 +0100

    Merge pull request #38 from f4bjh/37-task_wdt-task-watchdog-got-triggered
    
    37 - disable wdt_task under flag (is it really disable ?)

[33mcommit fea49e0d2486e7cdfe1378b2b651524b348d86da[m[33m ([m[1;31morigin/37-task_wdt-task-watchdog-got-triggered[m[33m, [m[1;32m37-task_wdt-task-watchdog-got-triggered[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 20:01:33 2025 +0100

    37 - disable wdt_task under flag (is it really disable ?)

[33mcommit c864143ded44e907e7c5c6d8608d789a20e4c1b5[m[33m ([m[1;31morigin/17-if-websocket-client-is-placed-in-a-javascript-file-it-fail-to-open-connection[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 09:49:58 2025 +0100

    17 - call java script in head tag of index.html

[33mcommit 8580a8469c45d07afe845ed84afe294ca04b07de[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 09:48:42 2025 +0100

    17 - align script.js with current java script  of index.html in main

[33mcommit 0dbb098eae4f1b6bd172b1e533f47f90acdd0689[m
Merge: e948040 89e7596
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 09:30:29 2025 +0100

    Merge branch 'main' into 17-if-websocket-client-is-placed-in-a-javascript-file-it-fail-to-open-connection

[33mcommit e9480409ec436579ec17067ee9d48c1f4d49508e[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Feb 4 09:28:53 2025 +0100

    17 - use EMBED_FILES everywhere to avoid null terminating string in binary array asm

[33mcommit e46b5eeb4a604a263628a3655c33a517b45b9b43[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 3 22:51:32 2025 +0100

    17 - restore pages

[33mcommit e6c9555d1dd70cb115409dff4c2f60c4b5dc7fed[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 3 22:46:53 2025 +0100

    17 - look at build/script.js.S, there is a null byte that ia added. To avoid the null byte, you can embed the file as binary data instead using EMBED_FILES.

[33mcommit 740a03d3428e246b44bfe27617f8737d5f58f6ec[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Feb 3 22:31:37 2025 +0100

    17 - issue is pointed. By default, esp idf environnement seems to add unwanted characters at the of the file. That leads to a syntax error by the java script interpreter. Wich one is it ? Why ? Would like to delete it git add .!

[33mcommit 89e7596bac320655adcc1d6c3222f5675a1d5346[m
Merge: 848e88c 462d734
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 01:19:25 2025 +0100

    Merge pull request #23 from f4bjh/22-place-data-fetching-in-a-specific-task
    
    22 place data fetching in a specific task

[33mcommit 462d73454015998b9a24150a8e0aade747c0100a[m[33m ([m[1;31morigin/22-place-data-fetching-in-a-specific-task[m[33m, [m[1;32m22-place-data-fetching-in-a-specific-task[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 00:52:35 2025 +0100

    22 - add log message in case of error when sending msg to ws client

[33mcommit 83580cc76118171c49ff40ae2f7ea068cd5be099[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 00:28:50 2025 +0100

    22 - clean

[33mcommit 1870b27ff50977f450c229f55718748054f4db24[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 00:09:27 2025 +0100

    22 - add some debug trace of CPU activity and wdt trig

[33mcommit 1bdc1aedf304eedf2d981b881bff89ea57040954[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu Jan 30 00:09:01 2025 +0100

    22 - deleted unneeded new line in web pages

[33mcommit 96eb3c9f503139269e0162a238236e6176131d22[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Jan 29 23:13:47 2025 +0100

    22 - improve log output as log level vebosity setting

[33mcommit 150da6307f3cc926148cdd54a0b7cd41c3d5a407[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Jan 29 08:28:20 2025 +0100

    22 - adapt to be compliant with sw architecture as described in #26

[33mcommit 786f073246626e10892c738297c9eb1c13c8389d[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 07:16:48 2025 +0100

    22 - not needed anymore

[33mcommit ca4c040e8e3d392a228d5ad9e598fed26ae36d53[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 01:33:50 2025 +0100

    22 - use a global DEFINE for the size of json string in allocate memory

[33mcommit eb1d063288293f742d1666eb03c3230900517575[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 01:20:13 2025 +0100

    22 - tune time task of web server, in order it gets all possible message in the queue

[33mcommit f531e7e1689f26c191f371f1b8e7efd69274de55[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 01:13:38 2025 +0100

    22 - declare global variable for chip_model and chip_revision to store permanent value inside js script

[33mcommit 0d938e7cf74d562802ff732175b17348e3177753[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 27 23:35:24 2025 +0100

    22 - add chip revision

[33mcommit 24a438478536662a4459c9d415090e44c93fbfd4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 27 22:15:43 2025 +0100

    22 - the server faile to get the client too early, so manage the receive of the Queue inside websocket server sending async function

[33mcommit 502888fac276a6a45a6892ec6d7758a294fc88e6[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jan 27 21:55:56 2025 +0100

    22 - allocate the correct number of char that a json string have, with all tabs, newline, and so on...

[33mcommit 4b82c177d9ec6433e106a1b562b98e7c648d24d0[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed Jan 15 08:10:10 2025 +0100

    22 - implement a default json data in case of no data received in queue

[33mcommit d841649fe50061d5a1f67a5577e90c87ab068900[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Jan 12 16:13:24 2025 +0100

    22 - flip to the real chip info

[33mcommit 24d4db5818ca266f81d8b62626122ae52c8a9445[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon Jul 29 08:48:29 2024 +0200

    22- queue management example

[33mcommit 6ede20acbf88893a38a42ae9b499dfb5ff8fdc49[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Jun 2 18:35:06 2024 +0200

    22 - add a task dedicated to data get

[33mcommit 149a85bd74147e4935c33b690eae82a3cb95317c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue Jan 14 08:26:03 2025 +0100

    22 - complete README with openocd error case :  esp_usb_jtag: could not find or open device
    
    22 - complete README with gdb command reset halt
    
    22 - complete README with gdb print string command
    
    22 - complete README with flash monitor
    
    22 - add tuto about log verbosity in README
    
    22 - complete README with make clean like cmd

[33mcommit 848e88ce500d799bf8986cf2b5089a1fcd4ed45e[m
Merge: ac9ba02 cd38a5f
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed Jan 29 08:38:00 2025 +0100

    Merge pull request #31 from f4bjh/30-add-ci
    
    30 add ci

[33mcommit cd38a5f21d6ff917760efb9d78901ebec889c7de[m[33m ([m[1;31morigin/30-add-ci[m[33m, [m[1;32m30-add-ci[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 16:40:56 2025 +0100

    30 - add qemu and rename build.yml to .github-ci.yml

[33mcommit 3f0b3dc40a917d3f1eccfe39afb7429416d518f2[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 16:20:36 2025 +0100

    30 - correct output firmware binary file

[33mcommit ddf428c082cf74b2ee0a60cdc11cf2fb3f0da7e8[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 16:04:58 2025 +0100

    30 - set IDF_TARGET

[33mcommit aebf1bb4debcdc544cacd4ffdfba6770b04f165a[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:56:02 2025 +0100

    30 - update esp-idf to 5.3.2

[33mcommit 2bdcc5c8151fe66c4ce82c3424af7ab393a85717[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:51:32 2025 +0100

    30 - Install latest CMake

[33mcommit ac9ba0256ec2de60e6289e7337ccad69a92de70a[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 17:28:44 2025 +0100

    Delete .github/workflows/build.yml

[33mcommit 83e8e2c90c7ff248f633582fdddacaaa56c1fdab[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:23:59 2025 +0100

    Launch CI on all branches

[33mcommit 90d6edca9b278bd515bb92eb67d9db5182dd636e[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:20:34 2025 +0100

    upload artifacts v2 is deprecated. Use v4

[33mcommit c33a3aca0a1984949167f5b8194426bd0df8646a[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:18:28 2025 +0100

    v2 is depecrated. Use v4

[33mcommit 4b2798098eb1a3843e15dba9e283adc6c0c74f5d[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:17:16 2025 +0100

    Update and rename main.yml to build.yml

[33mcommit f6db265d50dee9e42bebf2de71ea5ffe64b3dcb4[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue Jan 28 15:11:57 2025 +0100

    Create main.yml

[33mcommit cc77611c63b7398cd85138e8246937f8071d0276[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Jun 2 12:30:26 2024 +0200

    improve display of readme

[33mcommit ca81085b083b9accdc8df90247df8e5be44d62ee[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sun Jun 2 12:28:14 2024 +0200

    add opencd command in readme

[33mcommit 24c984fc5d687da927af35d9bf230bcca323db61[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu May 30 01:39:35 2024 +0200

    17 - websocket client in a separate js file

[33mcommit fcc684153bcf8dc9aa62b718f090b2a07909c87b[m
Merge: 405bbf6 755ebf2
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 30 00:25:42 2024 +0200

    Merge pull request #20 from f4bjh/14-move-draw-io-files-into-a-doc-folder
    
    14 - add doc folder

[33mcommit 755ebf2629f136938d0f0c15af3e87ca7a04dfb5[m[33m ([m[1;31morigin/14-move-draw-io-files-into-a-doc-folder[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu May 30 00:24:47 2024 +0200

    14 - add doc folder

[33mcommit 405bbf68053f88b5c0f2833d00be4b391766be5b[m
Merge: 8ed676a 34962b8
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 30 00:21:38 2024 +0200

    Merge pull request #19 from f4bjh/6-add-informations-into-home-page
    
    6 add informations into home page

[33mcommit 34962b84bafe34f86ff22edc0cfce8bd220eca9d[m[33m ([m[1;31morigin/6-add-informations-into-home-page[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue May 28 00:38:55 2024 +0200

    6 - add CPU info

[33mcommit f7db52a138d898e349e83097ce0ed8c465f34cf8[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Thu May 23 00:37:37 2024 +0200

    6 - implement a counter value, and display its value in the web page

[33mcommit 755a4e201273a2903c76ea8b262ec6123a624720[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat May 18 19:25:17 2024 +0200

    6 - add websoket server and js client

[33mcommit 6648e11a822c6f91cf86c6727e8698cbc15ba586[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue May 21 19:46:53 2024 +0200

    6 - complete README

[33mcommit 784762d9153acb3f7f7fe6b0fe27b34027889aa4[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat May 18 10:47:03 2024 +0200

    6 - add *.old in gitignore

[33mcommit 8ed676a8bdf7205e2fff1e24a66ddc3a6819c3f8[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat May 18 10:48:46 2024 +0200

    add minicom command in readme

[33mcommit c16ee8f14003e40698b270fb4a4e2e58dcf749ce[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Sat May 18 10:45:31 2024 +0200

    add opencd command in readme

[33mcommit 081ff749127fabd5edf444c1ca65e5a5eb7661d4[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri May 17 15:29:36 2024 +0200

    archi fw

[33mcommit 74340f146c26bdf1fd72af0c28fd1b1eda174276[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri May 17 11:08:41 2024 +0200

    Add files via upload

[33mcommit f9827b9919453976cdc6d92c4631ea3b95a23095[m
Merge: 2738d78 d7eda8e
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 16 23:12:37 2024 +0200

    Merge pull request #13 from f4bjh/5-indexhtml-unreachable
    
    move index.html to /

[33mcommit 2738d78e6829cfed919bee8ce474e4f4f4715ebb[m
Merge: a2f1d7a 764787a
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 16 23:11:32 2024 +0200

    Merge pull request #12 from f4bjh/8-frequency-value-not-really-readable
    
    improve readibility

[33mcommit a2f1d7a63001ba76aca3a57fed6aaf511a86638d[m
Merge: 39ad5e9 7d235ee
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 16 23:08:25 2024 +0200

    Merge pull request #11 from f4bjh/4-complete-abouthtml-page
    
    add info in about.html page

[33mcommit 7d235ee5982bc4b0c158926559d695c63cdb5b9e[m[33m ([m[1;31morigin/4-complete-abouthtml-page[m[33m, [m[1;32m4-complete-abouthtml-page[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 16 17:59:30 2024 +0200

    add info in about.html page

[33mcommit 764787a005e1c169ba3c8e6d1212c666ce77fbc2[m[33m ([m[1;31morigin/8-frequency-value-not-really-readable[m[33m, [m[1;32m8-frequency-value-not-really-readable[m[33m)[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Thu May 16 17:46:34 2024 +0200

    improve readibility

[33mcommit d7eda8e19d29ccd00c36df86841aa11224b1d666[m[33m ([m[1;31morigin/5-indexhtml-unreachable[m[33m, [m[1;32m5-indexhtml-unreachable[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed May 15 22:37:54 2024 +0200

    move index.html to /

[33mcommit 39ad5e92c0b5c5376e95587e1306b6e8faa6bcef[m
Merge: 4ae7abe b05be42
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 15 22:12:59 2024 +0200

    Merge pull request #2 from f4bjh/softAP-Wifi
    
    + wifi standalone access point
    + web server
    + fw update over the air (ota)

[33mcommit b05be421e2ba2ae225977178cad3d3e95883e341[m[33m ([m[1;31morigin/softAP-Wifi[m[33m, [m[1;32msoftAP-Wifi[m[33m)[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed May 15 22:00:39 2024 +0200

    register handles of all needed pages

[33mcommit d6f3433572240726bca961c814b46ffa82293c1b[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed May 15 20:18:04 2024 +0200

    source architecture

[33mcommit 4c601dddf8cfbdbaf0a08bfccf23753ea7ef9744[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 15 15:55:55 2024 +0200

    esthetic improvements

[33mcommit c4f7892760fbc800097eb13610e1c44729b8e117[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 15 15:05:58 2024 +0200

    improve power meter page

[33mcommit 13ea529aa41040de3168665d48618efb9e1b7dc6[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Wed May 15 14:04:25 2024 +0200

    improve frequency display

[33mcommit b7ed9f8845316ec6f520707206b300e7b8361ddc[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Wed May 15 08:07:28 2024 +0200

    add web site file in compilation process

[33mcommit f9fdda0c2517298f3974b28e3cef6389b6e689fa[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue May 14 16:47:42 2024 +0200

    keep menu in sub pages

[33mcommit 12a9ce88043947e5d1647143e43723e0d76f3e15[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue May 14 16:03:10 2024 +0200

    add gauge in rf power meter page

[33mcommit c7fe58e82613ebb88ac2bd4e56c289f9be4e9ceb[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Tue May 14 14:33:56 2024 +0200

    1st template of html pages

[33mcommit 6f054617ba381ecf6af2e8ad6158676ac40ef343[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue May 14 01:01:58 2024 +0200

    update sdkconfig
    
    idf.py menuconfig and go to :
    - choose 2 OTA in config partition table type
    - adapt flash size

[33mcommit 34372b17fbeefd4cdbb75183f243ef6c9863220c[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Tue May 14 01:01:48 2024 +0200

    implement a web server in standalone wifi AP

[33mcommit d36db42c255b42506f05709e4a50fdfeb967a9c2[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 22:08:42 2024 +0200

    add build in .gitignore

[33mcommit 4ae7abece5562a09ca552a846532acd1f7779205[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 21:42:07 2024 +0200

    some changes in project and filename

[33mcommit c7c60e6eaa898051a35aebfad472dcd7702b49bb[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 21:41:52 2024 +0200

    add .gitignore

[33mcommit 8932621220c44a131929548dc88048e0bd547099[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 21:36:42 2024 +0200

    add some usefull command

[33mcommit f66e4e03abc971baf37812f04629caff68d7ea5f[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 21:34:34 2024 +0200

    complete README

[33mcommit 8a4e1f0273d6a657e9ac8f36bcad416be1a706de[m
Author: f4bjh <f4bjh_95@yahoo.fr>
Date:   Mon May 13 21:25:53 2024 +0200

    start project

[33mcommit e4d65852029aed2c566d602b82efd3372b20ec96[m
Author: Fabrice DECROP LONGET <f4bjh_95@yahoo.fr>
Date:   Fri May 3 13:47:03 2024 +0200

    Initial commit
