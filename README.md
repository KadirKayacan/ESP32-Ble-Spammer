# ESP32 BLE Spammer

## Genel Bakış

Bu proje, ESP32 mikrodenetleyicisi üzerinde çalışan, C++ (Arduino framework + ESP-IDF BLE API) ile geliştirilmiş bir **Alt Seviye BLE Beacon Spammer** aracıdır. ESP32'nin Bluetooth Low Energy (BLE) donanımını **ham reklam paketi (raw advertisement) enjeksiyonu** modunda kullanarak, çevredeki telefon, tablet ve bilgisayarların **Bluetooth cihaz listelerinde** sahte cihazların görünmesini sağlar.

Araç, her yayın döngüsünde **rastgele bir MAC adresi** (Non-Resolvable Private Address standardına uygun) üretir ve bu adresle birlikte önceden tanımlanmış **10 farklı sahte cihaz ismini** yayar. Bu sayede hem cihaz isimleri hem de MAC adresleri sürekli değişerek gerçek bir cihaz envanteri yanılsaması oluşturur.

Bu proje; **BLE güvenliği farkındalığı**, **GAP (Generic Access Profile) reklam paketi yapısı öğrenimi**, **MAC adresi gizliliği (privacy)** ve **yetkili güvenlik testleri** amacıyla geliştirilmiştir. BLE spam / beacon flooding saldırılarının nasıl çalıştığını ve etkilerini anlamak için eğitim amaçlı bir laboratuvar aracıdır.

## Özellikler

- **10 farklı sahte BLE cihaz ismi** yayınlama (Matrix, trol, siber güvenlik temalı)
- **Ham BLE reklam paketi (Raw Advertisement)** enjeksiyonu (`esp_ble_gap_config_adv_data_raw` ile)
- Her yayın için **rastgele MAC adresi** üretimi (`esp_ble_gap_set_rand_addr`)
- **Non-Resolvable Private Address** standardına uygun MAC üretimi (ilk baytın üst 2 biti `00`)
- **ADV_TYPE_IND** (Connectable undirected advertising) modu ile gerçekçi cihaz taklidi
- **Tüm BLE kanallarında** yayın (`ADV_CHNL_ALL` → 37, 38, 39)
- Paket yapısına uygun **Flags** ve **Complete Local Name** AD (Advertising Data) alanları
- IEEE BLE standardına uygun **isim uzunluğu sınırı kontrolü** (maks 26 karakter)
- Seri port (115200 baud) üzerinden her yayın için MAC + isim çıktısı
- Paketler arası `delay(300)` ile telefonların cihaz listesini yakalamasına olanak tanıma

## Teknolojiler

- C++ (Arduino framework + ESP-IDF BLE API)
- ESP32 (ESP-WROOM-32 veya uyumlu, BLE destekli kartlar)
- Arduino IDE / PlatformIO
- Kütüphaneler:
  - `Arduino.h` (temel Arduino fonksiyonları)
  - `BLEDevice.h` (ESP32 BLE kütüphanesi)
  - `esp_gap_ble_api.h` (ESP-IDF BLE GAP API — ham reklam ve MAC yönetimi için)

## Kurulum

```bash
git clone https://github.com/KadirKayacan/ESP32-Ble-Spammer.git
cd ESP32-Ble-Spammer

# PlatformIO kullanıyorsan:
pio run --target upload

# Arduino IDE kullanıyorsan:
# 1. Arduino IDE'yi aç
# 2. Board Manager'dan "esp32" paketini kur (Espressif Systems)
# 3. Karta "ESP32 Dev Module" seç
# 4. Partition Scheme: "Default 4MB with spiffs" veya benzeri seç
# 5. main.cpp dosyasını aç ve yükle
```

> Not: `esp_ble_gap_config_adv_data_raw()` ve `esp_ble_gap_set_rand_addr()` fonksiyonları ESP-IDF'in düşük seviyeli BLE API'sidir ve ESP32 Arduino çekirdeği ile birlikte gelir. Harici bir kütüphane kurulumu gerekmez. Ancak **raw advertisement** yapabilmek için ESP32 çekirdeğinin güncel bir sürümü (2.0.x veya üzeri) önerilir.

## Kullanım

```bash
# PlatformIO ile derleme ve yükleme
pio run --target upload

# Seri monitörü aç (115200 baud)
pio device monitor -b 115200
```

Cihaz açıldıktan sonra herhangi bir komut girmeye gerek yoktur; kod otomatik olarak BLE yayınına başlar. Seri porttan aşağıdaki gibi bir çıktı görürsünüz:

```text
==================================================
   ALT SEVIYE BLE BEACON SPAMMER AKTIF!
   MAC Adresleri maskeleniyor...
==================================================

[YAYINDA] MAC: 3A:1F:8C:D2:44:91 -> 01_Hacker_Test_Node
[YAYINDA] MAC: 0B:7E:23:A1:9F:55 -> 02_Zion_Ana_Sunucu
[YAYINDA] MAC: 2C:88:D4:6E:11:B3 -> 03_Cihaz_Ele_Gecirildi
[YAYINDA] MAC: 1D:45:AA:37:C2:08 -> 04_Ucretsiz_Bitcoin_Cuzdani
...
```

Çevredeki bir telefonun Bluetooth menüsünü açtığınızda, `01_Hacker_Test_Node`, `03_Cihaz_Ele_Gecirildi`, `05_EGM_Siber_Devriye` gibi sahte cihazların listelendiğini görürsünüz. MAC adresleri sürekli değiştiği için aynı isimli cihazlar farklı MAC'lerle tekrar görünebilir.

### Cihaz İsimlerini Özelleştirme

`main.cpp` dosyasındaki `deviceNames[]` dizisini düzenleyerek kendi cihaz isimlerinizi yayabilirsiniz:

```cpp
const char* deviceNames[] = {
  "Benim_Test_Cihazim_01",
  "Benim_Test_Cihazim_02",
  // ...
};
const int totalNames = 10; // Dizi boyutunu güncelleyin
```

> Dikkat: BLE reklam paketi toplam **31 byte** ile sınırlıdır. Flags alanı 3 byte, AD header'ları 2 byte yer kapladığı için isim **en fazla 26 karakter** olabilir. Kod, 26 karakterden uzun isimleri otomatik olarak kırpar.

### Yayın Süresini Ayarlama

Her cihaz isminin ekranda ne kadar süre kaldığını `delay(300)` değerini değiştirerek ayarlayabilirsiniz. Daha uzun süre → telefonların yakalama olasılığı artar.

## Proje Yapısı

```text
ESP32-Ble-Spammer/
├── src/
│   └── main.cpp          # Ana uygulama (isim listesi + BLE reklam enjeksiyonu)
├── platformio.ini        # PlatformIO yapılandırması
├── README.md
├── .gitignore
```

Arduino IDE kullanıyorsan dosya yapısı şu şekilde olur:

```text
ESP32-Ble-Spammer/
├── esp32_ble_spammer.ino
├── README.md
├── .gitignore
```

## Teknik Detaylar

### BLE Reklam Paketi Yapısı

Yayınlanan paket, Bluetooth Core Specification'a uygun **AD (Advertising Data)** yapısına sahiptir. Her AD yapısı `[Uzunluk][Tip][Veri]` formatındadır:

| Alan | Uzunluk | Tip | Açıklama |
|------|---------|-----|----------|
| Flags | 3 byte | `0x01` | `0x02 0x01 0x06` → LE General Discoverable + BR/EDR Not Supported |
| Complete Local Name | 2 + N byte | `0x09` | Cihaz ismi (maks 26 karakter) |

Toplam paket boyutu **31 byte**'ı geçemez (BLE standardı).

### MAC Adresi Üretimi

Her yayın için 6 byte rastgele üretilir ve ilk byte'ın üst 2 biti `00` yapılır:

```cpp
rand_addr[0] &= 0x3F;  // Üst 2 biti temizle
```

Bu, Bluetooth Core Specification'daki **Non-Resolvable Private Address** formatına uyar:
- `00` → Non-resolvable private address
- `01` → Resolvable private address
- `10` → Reserved
- `11` → Static random address

Bu sayede MAC adresi her yayında değişir ve gerçek cihaz taklidi güçlenir.

### Reklam Parametreleri

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| `adv_int_min` | `0x20` (32 × 0.625 ms = 20 ms) | Minimum reklam aralığı |
| `adv_int_max` | `0x40` (64 × 0.625 ms = 40 ms) | Maksimum reklam aralığı |
| `adv_type` | `ADV_TYPE_IND` | Bağlanılabilir, yönlendirilmemiş reklam |
| `own_addr_type` | `BLE_ADDR_TYPE_RANDOM` | Rastgele MAC kullan |
| `channel_map` | `ADV_CHNL_ALL` | 37, 38 ve 39. kanalların tümünde yayınla |

## Sınırlamalar

- BLE reklam paketi **31 byte** ile sınırlıdır; bu nedenle isimler kısaltılabilir.
- ESP32, aynı anda yalnızca **tek bir reklam seti** yayabilir; bu yüzden her isim sırayla yayınlanır (paralel değil).
- Bazı işletim sistemleri (özellikle iOS), aynı MAC adresini kısa sürede çok sık görürse cihazı listede filtreleyebilir.
- `esp_ble_gap_set_rand_addr()` bazı ESP32 çekirdek sürümlerinde reklam başlamadan önce çağrılmalıdır; kod bu sırayı takip eder.

## Güvenlik ve Sorumlu Kullanım

Bu araç **yalnızca sahibi olduğunuz ortamlarda veya yazılı test izni aldığınız alanlarda** kullanılmalıdır. BLE beacon flooding / cihaz ismi spam'i, hedef cihazların Bluetooth listelerini doldurarak:

- Kullanıcı deneyimini bozabilir,
- Kullanıcıları sahte cihazlara yönlendirmeye zemin hazırlayabilir,
- Cihaz pilini gereksiz yere tüketebilir,
- Yasal olarak **yetkisiz müdahale** suçu oluşturabilir.

Başkalarına ait cihazlara yönelik izinsiz BLE müdahalesi çoğu ülkede (Türkiye'de 5237 sayılı TCK ve 5809 sayılı Elektronik Haberleşme Kanunu kapsamında) yasalara aykırıdır. Bu proje yalnızca **eğitim, farkındalık ve yetkili güvenlik araştırması** amacıyla geliştirilmiştir. Kullanıcı, aracın kullanımından doğacak tüm yasal sorumluluğu kabul eder.

