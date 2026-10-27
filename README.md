# Landy Client 1.5 stable-beta — kaynak kodu

## Türkçe

Landy Client, BestClient ve DDNet/TeeWorlds tabanlı bir istemci forkudur. Bu klasör Git geçmişi ve üretilmiş istemci/sunucu dosyaları olmadan paylaşılabilir kaynak kopyasıdır. Windows x64 derlemesi için gereken üçüncü taraf bağlantı ve çalışma zamanı dosyaları `ddnet-libs/` ve `noxy-libs/` altında bulunur; bunların kendi lisansları ilgili klasörlerde korunmuştur.

### Özellikler

- Türkçe ve İngilizce arayüz seçimi.
- İsteğe bağlı sohbet çevirisi: Google, Lingva ve MyMemory servisleri; hizmetler erişilemezse yedekleme davranışı.
- DDNet/BestClient sunucu tarayıcısı, editör ve oyun istemcisi.
- Key tap ve gecikme/input seçenekleri mevcuttur; yayın varsayılanlarında kapalıdır.

Çeviri kalitesi ve erişilebilirliği üçüncü taraf servis sağlayıcılarına bağlıdır. Özelliklerin tamamı bu yayın hazırlığında gerçek sunucuda tek tek test edilmemiştir; doğrulama durumu `RELEASE_AUDIT.md` içindedir.

### Gizlilik ve ağ erişimi

Sohbet çevirisini kullandığınızda seçili mesaj metni çeviri sağlayıcısına gönderilir. Diğer bağlantılar ve varsayılan durumları `RELEASE_AUDIT.md` içindeki tabloda açıklanmıştır. İstemci göstergesi ve Klanlar özellikleri üçüncü taraf BestClient altyapısına bağlanabilir; yayın varsayılanları için kararı kullanıcı vermelidir.

Bu Landy Client derlemesinde otomatik sürüm indirme ve uygulama devre dışıdır. Ana menüdeki `Check update` denetimi bu projeye ait olmayan eski bir üçüncü taraf sürüm adresinden bilgi sorgular; adres değiştirilmedi. Ayrıntı ve karar bekleyenler audit dosyasındadır.

### Uyarı

Key tap, input, gecikme ve performans seçenekleri sunucu veya topluluk kurallarını ihlal edebilir. Varsayılan olarak kapalı olsalar da açmadan önce bağlandığınız sunucunun kurallarını kontrol edin.

### Windows x64 derleme

Gerekenler: Visual Studio 2022 C++ Build Tools ve Windows SDK, CMake, Ninja veya MSBuild, Python 3 ve Rust/Cargo. Kaynak paketinde `ddnet-libs` bağımlılığı bulunduğundan Git/submodule indirmesi gerekmez.

Visual Studio generator ile örnek:

    cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DDEV=OFF -DDOWNLOAD_GTEST=OFF
    cmake --build build --config Release --target game-client DDNet-Server

Ayrıntılı bağımlılık, lisans ve test durumu için `CREDITS.md`, `LICENSE`, `license.txt` ve `RELEASE_AUDIT.md` dosyalarına bakın.

## English

Landy Client is a fork based on BestClient and DDNet/Teeworlds. This folder is a shareable source copy without Git history or generated client/server executables. Third-party link and runtime files required for the Windows x64 build are included under `ddnet-libs/` and `noxy-libs/`, with their upstream license notices retained.

### Features

- Turkish and English interface selection.
- Optional chat translation using Google, Lingva and MyMemory, with fallback behavior when a provider is unavailable.
- DDNet/BestClient server browser, editor and game client.
- Key-tap and input/latency options are available and disabled in the release defaults.

Translation quality and availability depend on third-party providers. Not every feature was individually tested on a live server during this release preparation; see `RELEASE_AUDIT.md` for verification status.

### Privacy and network access

When chat translation is used, selected message text is sent to the translation provider. Other connections and their defaults are documented in `RELEASE_AUDIT.md`. Client Indicator and Clans can connect to third-party BestClient infrastructure; review the audit before enabling related features.

Automatic version download and application are disabled in this Landy Client build. The main-menu `Check update` action still queries an older third-party release endpoint that does not belong to this project; it was left unchanged. See the audit for details and pending decisions.

### Warning

Key-tap, input, latency and performance options may violate server or community rules. They are disabled by default; check the rules of the server you join before enabling them.

### Windows x64 build

Requirements: Visual Studio 2022 C++ Build Tools and Windows SDK, CMake, Ninja or MSBuild, Python 3, and Rust/Cargo. The required `ddnet-libs` dependency is included, so Git/submodule downloads are not needed.

Example with the Visual Studio generator:

    cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DDEV=OFF -DDOWNLOAD_GTEST=OFF
    cmake --build build --config Release --target game-client DDNet-Server

See `CREDITS.md`, `LICENSE`, `license.txt` and `RELEASE_AUDIT.md` for licensing, dependencies and test status.

