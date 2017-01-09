#pragma once

#include <string>
#include <set>

#include "ChatExporter.h"

class Settings;
class WhatsappChat;
class WhatsappMessage;

class ChatExporterHtml : public ChatExporter
{
private:
	Settings &settings;
	std::string templateHtml;
	std::string templateText;
	std::string templateAuthor;
	std::string templateImage;

	std::string buildMessages(WhatsappChat &chat, std::set<int> &usedEmoticons);
	void replacePlaceholder(std::string &html, const std::string &placeholder, const std::string &text);
	std::string convertMessageToHtml(WhatsappMessage &message, std::set<int> &usedEmoticons);
	std::string buildEmoticonStyles(const std::set<int> &usedEmoticons);

public:
	ChatExporterHtml(Settings &settings, const std::string &templateHtml, const std::string &templateText, const std::string &templateAuthor, const std::string &templateImage);
	~ChatExporterHtml();

	void exportChat(WhatsappChat &chat, const std::string &filename);
};
