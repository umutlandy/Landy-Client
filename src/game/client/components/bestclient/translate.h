/* Copyright © 2026 BestProject Team */
#ifndef GAME_CLIENT_COMPONENTS_BESTCLIENT_TRANSLATE_H
#define GAME_CLIENT_COMPONENTS_BESTCLIENT_TRANSLATE_H

#include <game/client/component.h>
#include <game/client/components/chat.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

class CTranslate;

class ITranslateBackend
{
public:
	virtual ~ITranslateBackend() = default;
	virtual const char *EncodeSource(const char *pSource) const;
	virtual const char *EncodeTarget(const char *pTarget) const;
	virtual const char *Name() const = 0;
	virtual std::optional<bool> Update(CTranslateResponse &Out) = 0;
	virtual bool IsRateLimited() const { return false; }
	virtual int LastResponseTimeMs() const { return 0; }
};

class CTranslate : public CComponent
{
	class CTranslateJob
	{
	public:
		enum class EType
		{
			CHAT_LINE = 0,
			OUTGOING_CHAT,
		};

		EType m_Type = EType::CHAT_LINE;
		std::unique_ptr<ITranslateBackend> m_pBackend = nullptr;
		CChat::CLine *m_pLine = nullptr;
		std::shared_ptr<CTranslateResponse> m_pTranslateResponse = nullptr;
		int m_Team = 0;
		char m_aOriginalText[MAX_LINE_LENGTH] = "";
		char m_aTextToTranslate[MAX_LINE_LENGTH] = "";
		char m_aOutgoingPrefix[MAX_LINE_LENGTH] = "";
		bool m_RespectIgnoredIncomingLanguages = false;
		int m_ServiceIndex = 0;
		int m_ServiceAttempts = 0;
	};
	std::vector<CTranslateJob> m_vJobs;
	int m_aServiceHealth[3] = {1, 1, 1};
	int m_aServiceLastSuccessMs[3] = {};
	char m_aaServiceLastError[3][96] = {};
	int64_t m_NextRequestTime = 0;
	int64_t m_RateLimitUntil = 0;

	static void ConTranslate(IConsole::IResult *pResult, void *pUserData);
	static void ConTranslateId(IConsole::IResult *pResult, void *pUserData);
	static void ConToggleTranslate(IConsole::IResult *pResult, void *pUserData);
	std::unique_ptr<ITranslateBackend> CreateBackend(const char *pText, const char *pSourceLanguage, const char *pTargetLanguage, int ServiceIndex) const;
	const char *IncomingSourceLanguage() const;
	const char *IncomingTargetLanguage() const;
	const char *OutgoingSourceLanguage() const;
	const char *OutgoingTargetLanguage() const;
	bool IsIgnoredIncomingLanguage(const char *pLanguage) const;
	bool ShouldTranslateOutgoingChat(const char *pText) const;
	bool HasPendingJobs() const;
	bool CanStartRequest() const;
	void TranslateLine(CChat::CLine &Line, bool ShowProgress, bool RespectIgnoredIncomingLanguages);

public:
	int Sizeof() const override { return sizeof(*this); }

	void OnConsoleInit() override;
	void OnRender() override;

	void Translate(int Id, bool ShowProgress = true);
	void Translate(const char *pName, bool ShowProgress = true);
	void Translate(CChat::CLine &Line, bool ShowProgress = true);
	bool TryTranslateOutgoingChat(int Team, const char *pText);

	void AutoTranslate(CChat::CLine &Line);
	int ServiceHealth(int Service) const { return Service >= 0 && Service < 3 ? m_aServiceHealth[Service] : 2; }
	int ServiceLastSuccessMs(int Service) const { return Service >= 0 && Service < 3 ? m_aServiceLastSuccessMs[Service] : 0; }
	const char *ServiceLastError(int Service) const { return Service >= 0 && Service < 3 ? m_aaServiceLastError[Service] : ""; }
};

#endif // GAME_CLIENT_COMPONENTS_BESTCLIENT_TRANSLATE_H
