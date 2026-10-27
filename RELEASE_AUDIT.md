# Release audit — 1.5 stable-beta

## Source and build

- Source export is a plain folder without `.git`, build outputs, user profile files, patch files, duplicate BestClient checkout, or duplicate `ddnet-libs-master`.
- `ddnet-libs/` is included as vendor content and `.gitmodules` was omitted. Windows x64 `.lib` link inputs and required third-party runtime `.dll` files are retained because CMake requires them to build and run the clean client. These are third-party dependencies, not Landy Client build outputs.
- Release-specific source changes in this export disable updater archive downloads and updater process execution. The original working tree was clean before export and was not modified, per the source-path read-only instruction.

## External endpoint inventory

| Address | Purpose / data involved | Operator | Default and trigger |
|---|---|---|---|
| `master1.ddnet.org`–`master4.ddnet.org` | DDNet server list requests; network request metadata | DDNet | Server browser default |
| `master1.ddnet.org/ddnet/15/register` | Register this client/server with the DDNet master service | DDNet | When hosting/registering a server |
| `info.ddnet.org` | DDNet community/server metadata | DDNet | Server browser metadata lookup |
| `info.ddnet.org/censor.json` | Name/censor list data | DDNet | When client/server censor data is loaded |
| `maps.ddnet.org` | Download requested maps | DDNet | Default URL; when a needed map is missing |
| `skins.ddnet.org` | Download requested player/community skins | DDNet | Default URL; when user requests a skin |
| `master.bestclient.fun` | Alternate server-list mirror | BestClient | `bc_mastersrv=0` by default; when enabled |
| `150.241.70.188:8778` (UDP) | Client Indicator presence packet with version/player/server identifying fields | BestClient-era third-party service | **Always enabled** (`bc_client_indicator=1`, fixed); transmits while connected |
| `https://150.241.70.188:8779/users.json` | Client Indicator browser data | BestClient-era third-party service | Default URL; when indicator/browser data is refreshed |
| `https://150.241.70.188:8779/token.json` | Client Indicator shared-token bootstrap | BestClient-era third-party service | Default URL; on indicator initialization |
| `clans.bestclient.fun` | Clans API account and clan actions | BestClient | Clans page enabled by default; requests on page/actions |
| `translate.google.com` | Chat text and source/target language | Google | Translation service 0; when translation is requested |
| User-configured Lingva host | Chat text and source/target language | Host operator chosen by user | Lingva fallback; empty host list by default means it is not used until configured |
| `api.mymemory.translated.net` | Chat text and language pair | MyMemory | Translation fallback/service selection; when requested |
| `ftapi.pythonanywhere.com` | Chat text and target language | Third-party hosted translation API | Legacy/alternate translation path when selected |
| `api.github.com/repos/Mel0man4ik/milk-client/releases` | Update manifest request; client version/user-agent | Milk Client project/GitHub | Main-menu and settings `Check update`; automatic update default is off. Download/apply code is disabled in this export. **Address left unchanged for owner decision.** |
| `update.tclient.app/info.json` | TClient version/update metadata | TClient | Legacy version-check path in code; archive application disabled in this export |
| `lrclib.net` | Song, artist, album and duration query for lyrics | LRCLIB | Music-player lyrics lookup when requested |
| `best-client-api.vercel.app` | Player/skin image assembly request | BestClient-era hosted API | When the related Discord/media feature requests it |
| `media.giphy.com`, `media1.giphy.com`, `i.imgur.com` | Download media preview linked in chat | Giphy / Imgur | Only when chat preview is used |
| `raw.githubusercontent.com/SollyBunny/ddnet-custom-communities/...` | Community-list JSON | Community-list maintainer/GitHub | TClient default URL; on community list refresh |
| `eternal-gores.com`, `ru.ddnet.org` | Player score/profile lookup | Server/community operators | Only when points/profile display is requested on supported servers |
| `kog.tw`, `uniqueclan.net` | Player profile/rank lookup | KOG / UniqueClan | When corresponding player/profile lookup is requested |
| `discord.gg/*`, `t.me/bestddnet`, `bestclient.fun`, `tclient.app` | Open community/project pages | BestClient / TClient | User clicks the related community/about link; no automatic chat payload |
| User-configured voice/Twitch hosts | Voice packets and Twitch chat connection | User-selected server/service | Only when configured and enabled |

The endpoint list is based on active code/default configuration inspection, not packet capture. User-configured URLs and the DDNet game/master protocol can add connections not represented by HTTP literals above.

## Defaults and privacy

- Clean first-run package contains no personal settings, saved server list, demos, screenshots, ghosts, logs or translation cache.
- `bc_auto_update=0`; input mode `bc_inputs=0`; key tap latch `bc_key_tap_latch=0`; precise FPS limiter `bc_precise_fps_limit=0`; automatic incoming/outgoing translation `0`.
- Client Indicator is not opt-out in this source (`bc_client_indicator` range/default is fixed at `1`) and sends identifying presence data to the third-party IP listed above. This should be reconsidered by the owner before public release.
- Clans UI is enabled by default and points to BestClient's API. Default changes are left to the owner as requested.
- Translation requests transmit selected chat text to the chosen provider. Do not translate sensitive/private chat.

## Brand, licensing and provenance

- Main client keeps the established executable filename `DDNet.exe` to preserve data/config/server-launch behavior. Windows version metadata is branded Landy Client and uses the Landy L icon. Runtime integration beyond the tested launch is not confirmed.
- CatClient has a local permissive license file; no direct CatClient code/assets were found by name or active-source search in this tree. All existing `license.txt` and per-data-folder notices are preserved.
- `meow-input.patch` was found outside the Git working tree. Its author/source/license cannot be verified locally; the patch is omitted but related source changes remain. This is an unresolved provenance risk.
- The main-menu update check still targets Milk Client's GitHub release repository. The endpoint is unchanged, and no update archive can be downloaded or executed by the prepared client.

## Verification status

- Clean Windows x64 Release configure/build from this folder: passed using Visual Studio 2022 x64, `DEV=OFF`, tests/downloaded GTest disabled. Built `game-client` and `DDNet-Server`; the packaged `DDNet.exe` SHA-256 matches the clean source-build executable.
- Packaged client starts in an isolated empty profile without a missing-DLL/runtime error; the window title and main menu show Landy Client branding and the two language flags.
- Menu language-flag click behavior could not be reliably confirmed using automated UI input. Settings flags were visible in the earlier interface capture; selection persistence was not reconfirmed in the final package.
- Server browsing/join, map load, translation request, editor, local server, and 5–10 minute gameplay stability were not tested; manual verification is required.
- Source privacy scan found no local user-profile paths or secrets. E-mail-shaped matches are third-party copyright/contact notices in vendored headers; IP matches are documented hard-coded service endpoints, including the always-on Client Indicator. Generic protocol/config terms such as `token` and `password` are not credentials.
- The source export contains no individual file larger than 50 MB. The 53 required third-party `.lib` link-inputs and 12 runtime `.dll` dependencies are retained; no generated client/server `.exe` is present in the source export.

## Owner decisions before public release

- Decide whether to keep the fixed, always-on third-party Client Indicator and default-enabled BestClient Clans integration.
- Decide whether the menu `Check update` action should continue querying the legacy Milk Client release endpoint.
- Confirm the provenance/license for `meow-input.patch` and the origin of the related integrated code before redistributing it. The patch itself is excluded; its local author/license could not be established.
- CatClient has a local permissive license, and no direct CatClient code/assets were identified in this source tree. Preserve the license and re-check provenance if any CatClient material is later added.
