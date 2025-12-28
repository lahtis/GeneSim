# GeneSim NameGen v2.0.0 (NewYear-Edition)

**GeneSim NameGen** on korkean suorituskyvyn komentorivityökalu, joka on suunniteltu historiallisen tarkkojen 1800-luvun suomalaisten henkilöhahmojen luomiseen. Ohjelmassa on deterministinen satunnaisgeneraattori, painotettu yhteiskunnallinen logiikka ja tuki monimutkaisille perherakenteille.

---

## 🛠️ Virallinen komentorivikäyttöliittymä

### Käyttö
`namegen [asetukset]`

### Dokumentaatio
* **`--list-periods`, `-lp [fi|en]`** Näytä 11 historiallista aikakautta ja yksityiskohtaiset käyttöohjeet suomeksi tai englanniksi.

### Generointi ja logiikka
* **`--period <1-11>`** Valitse tietty historiallinen aikakausi (1: vanhin, 11: uusin).
* **`--year <vuosi>`** Aseta simulaatiovuosi (esim. `--year 1880`).
* **`--weights`, `-w`** Käytä historiallisia painokertoimia (**päällä oletuksena**).
* **`--no-weights`** Käytä tasaista jakaumaa (kaikilla nimillä on sama todennäköisyys).
* **`--count <luku>`** Luotavien nimien, pariskuntien tai perheiden määrä.
* **`--seed <luku>`** Aseta tietty siemenluku (seed) toistettavia tuloksia varten.

### Generointitilat
* **`--single`** Luo yksittäisiä nimiä (oletus).
* **`--couple`** Luo pariskuntia (yhdistetty mies ja nainen).
* **`--family`** Luo kokonaisia perheitä (vanhemmat ja lapset).
* **`--gender <male|female|both>`** Pakota sukupuoli luotaville nimille.

### Lisäasetukset
* **`--middle-chance <0-100>`** Toisen nimen todennäköisyys (%).
* **`--shared-surname`** Pakota pariskunnat/perheet käyttämään samaa sukunimeä.
* **`--maiden-name`** Näytä vaimojen tyttönimet käyttäen **"née"**-etuliitettä.
* **`--force-manual`** Poista automaattinen aikakausipohjainen logiikka käytöstä.
* **`--age`, `-A`** Näytä syntymävuodet (vaatii `--year` asetuksen).

### Tulostus ja muotoilu
* **`--output <plain|csv|json>`** Valitse tulostemuoto (teksti, CSV tai JSON).
* **`--stdout`** Pakota tuloste konsoliin, vaikka tiedostotallennus olisi päällä.
* **`--log [on|off]`** Ota käyttöön tai poista käytöstä lokitus `generator.log` -tiedostoon.

---

## 📝 Huomioitavaa v2.0.0-versiossa

* **Painotus:** Ohjelma käyttää nyt `--weights`-asetusta oletuksena, mikä tekee nimistä historiallisesti uskottavia (yleisimmät nimet esiintyvät useammin).
* **Lokitus:** Voit kytkeä lokitiedoston pois päältä `--log off` -komennolla, jotta `generator.log` ei kasva liian suureksi.
* **Aikakaudet:** Uusi `--list-periods` on keskeinen työkalu 11 eri historiallisen aikakauden hallintaan.

---

## ⚙️ Rakentaminen ja tekniset vaatimukset

* **Tietokantastandardi:** v2.0.0 vaatii tiukasti muotoillut CSV-tiedostot (Etunimet: 7 saraketta, Sukunimet/Ammatit: 11 saraketta).
* **Datan hallinta:** Käytä `gs_data_manager.py` -sovellusta CSV-tiedostojen automaattiseen korjaamiseen ja validointiin.
* **Testaus:** Käytä `gs_integrity_check.py` -sovellusta varmistaaksesi ohjelman vakauden kaikilla 11 aikakaudella.
* **JSON-moottori:** Hyödyntää `cJSON`-kirjastoa yhteiskuntaluokkien ja ammattien painotuksessa.
* **Arkkitehtuuri:** Optimoitu C99-koodi 512 tavun turvapuskureilla string-muotoiluja varten.
* **Lisenssi:** GPL v3.0 (Sisältää cJSON-kirjaston MIT-lisenssillä).