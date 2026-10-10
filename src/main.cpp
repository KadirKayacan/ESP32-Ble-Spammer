#include "Arduino.h"
#include "BLEDevice.h"
#include "esp_gap_ble_api.h"

// Telefonların Bluetooth menüsünde listelenecek sahte cihaz isimleri
const char* deviceNames[] = {
  "01_Hacker_Test_Node", 
  "02_Zion_Ana_Sunucu", 
  "03_Cihaz_Ele_Gecirildi", 
  "04_Ucretsiz_Bitcoin_Cuzdani",
  "05_EGM_Siber_Devriye", 
  "06_Arkani_Don_Bana_Bak", 
  "07_Baglanirsan_Coker", 
  "08_Sistem_Hatasi_0x99",
  "09_Matrix_Giris_Kapisi", 
  "10_Bilinmeyen_Cihaz"
};
const int totalNames = 10;

// Alt Seviye Reklam (Advertising) Parametreleri
static esp_ble_adv_params_t adv_params = {
    .adv_int_min       = 0x20, 
    .adv_int_max       = 0x40, 
    .adv_type          = ADV_TYPE_IND, // Bağlanılabilir (Connectable) Cihaz Modu
    .own_addr_type     = BLE_ADDR_TYPE_RANDOM, // Rastgele MAC adresi
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void setup() {
  Serial.begin(115200);
  delay(500); 
  
  // Arka plan görevlerinin yönetimi için ana servisi isimsiz başlat
  BLEDevice::init("");
  
  Serial.println("\n==================================================");
  Serial.println("   ALT SEVIYE BLE BEACON SPAMMER AKTIF!");
  Serial.println("   MAC Adresleri maskeleniyor...");
  Serial.println("==================================================\n");
}

void loop() {
  for (int i = 0; i < totalNames; i++) {
    // Yeni pakete geçmeden önce yayını durdur
    esp_ble_gap_stop_advertising();
    delay(10); 
    
    // 1. Bluetooth standartlarına uygun rastgele bir MAC Adresi üret
    uint8_t rand_addr[6];
    for (int j = 0; j < 6; j++) {
      rand_addr[j] = random(0, 256);
    }
    // Non-Resolvable Private Address standardı (İlk baytın en üst iki biti 00 olmalıdır)
    rand_addr[0] &= 0x3F; 
    
    // Üretilen sahte MAC adresini uygula
    esp_ble_gap_set_rand_addr(rand_addr);
    
    // 2. Ham Reklam (Raw Advertisement) Paketini oluştur
    uint8_t adv_data[31];
    int offset = 0;
    
    // Flags (Cihazın keşfedilebilir olduğunu belirtir)
    adv_data[offset++] = 0x02; 
    adv_data[offset++] = 0x01; 
    adv_data[offset++] = 0x06; 
    
    // İsim (Complete Local Name)
    int nameLen = strlen(deviceNames[i]);
    if (nameLen > 26) nameLen = 26; 
    
    adv_data[offset++] = nameLen + 1; 
    adv_data[offset++] = 0x09; 
    memcpy(&adv_data[offset], deviceNames[i], nameLen);
    offset += nameLen;
    
    // 3. Paketi yükle ve yayını başlat
    esp_ble_gap_config_adv_data_raw(adv_data, offset);
    esp_ble_gap_start_advertising(&adv_params);
    
    // Terminal ekranına bilgi yazdır
    Serial.print("[YAYINDA] MAC: ");
    for(int m=0; m<6; m++) { 
      Serial.printf("%02X", rand_addr[m]); 
      if(m<5) Serial.print(":"); 
    }
    Serial.print(" -> ");
    Serial.println(deviceNames[i]);
    
    // Telefonların cihaz listesinde bu ismi yakalayabilmesi için bekleme süresi
    delay(300); 
  }
}