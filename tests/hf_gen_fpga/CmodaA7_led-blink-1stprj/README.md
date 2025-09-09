# Projet Vivado : LED Blinky sur Cmod A7

Ce projet montre comment créer un design simple sur **Cmod A7** avec Vivado 2024.2 pour faire clignoter une LED en utilisant un **Clocking Wizard**.

---

## Étape 1 — Création du projet Vivado

1. **Lancer Vivado 2024.2 (64-bit)**.
2. **Créer un nouveau projet** : `Create Project`.
3. **Project Name** : `CmodA7_blinky`, choisir un dossier sans espaces.
4. **Project Type** :  
   - RTL Project  
   - Cocher **Do not specify sources at this time**
5. **Default Part / Board** :  
   - **Boards** → sélectionner **Cmod A7-35T** (ou 15T selon la carte)  
   - Ou **Parts** → rechercher `xc7a35t` (vérifier package & speed grade)
6. Le projet est créé, vide, avec Flow Navigator prêt pour ajouter sources et contraintes.

---

## Étape 2 — Ajouter le Clocking Wizard IP

1. **Ouvrir l’IP Catalog** → rechercher `clock` → double-cliquer **Clocking Wizard**.
2. **Configurer le Clocking Wizard** :  
   - **Input Clock** : 12.000 MHz  
   - **Output Clock** : activer `clk_out1`, par exemple 25 MHz  
   - Laisser les autres sorties désactivées.
3. **Nom de l’instance** : `clk_wiz_0`.
4. Générer les *output products*.

---

## Étape 3 — Top-level VHDL (blinky LED)

```vhdl
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity top_blinky is
  Port (
    clk_pin : in  std_logic;   -- horloge externe (12 MHz)
    led_o   : out std_logic    -- sortie vers LED
  );
end top_blinky;

architecture RTL of top_blinky is
  component clk_wiz_0
    port (
      clk_in1  : in  std_logic;
      clk_out1 : out std_logic;
      reset    : in  std_logic;
      locked   : out std_logic
    );
  end component;

  signal clk25   : std_logic;
  signal reset_n : std_logic := '0';
  signal locked  : std_logic;

  signal counter : unsigned(24 downto 0) := (others => '0');
  signal led_reg : std_logic := '0';

begin
  -- Instanciation du Clocking Wizard
  u_clk : clk_wiz_0
    port map (
      clk_in1  => clk_pin,
      clk_out1 => clk25,
      reset    => reset_n,
      locked   => locked
    );

  -- Process de clignotement
  process(clk25)
  begin
    if rising_edge(clk25) then
      counter <= counter + 1;
      if counter = 12500000 then  -- 25 MHz / 12.5M = 2 Hz → toggle toutes 0.5 s
        led_reg <= not led_reg;
        counter <= (others => '0');
      end if;
    end if;
  end process;

  led_o <= led_reg;

end RTL;
```

---

## Étape 4 — Fichier de contraintes XDC

```tcl
##=============================================================
## XDC minimal pour projet "top_blinky" Cmod A7
##=============================================================

## Clock input (12 MHz oscillator sur L17)
set_property PACKAGE_PIN L17 [get_ports clk_pin]
set_property IOSTANDARD LVCMOS33 [get_ports clk_pin]
create_clock -period 83.333 -name sys_clk -waveform {0 41.666} [get_ports clk_pin]

## LED0 (user LED sur A17)
set_property PACKAGE_PIN A17 [get_ports led_o]
set_property IOSTANDARD LVCMOS33 [get_ports led_o]
```

---

## Étape 5 — Synthèse, implémentation et programmation

1. **Run Synthesis** → vérifier qu’il n’y a pas d’erreurs.
2. **Run Implementation** → vérifier le timing summary.
3. **Generate Bitstream** → produire le fichier `.bit`.
4. **Program Device** :  
   - Flow Navigator → Hardware Manager → Open Target → Auto Connect  
   - Sélectionner le bitstream → OK
5. **Résultat attendu** : la LED0 clignote à environ 1 Hz.

---

