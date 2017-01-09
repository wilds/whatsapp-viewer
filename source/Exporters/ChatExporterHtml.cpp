#include <algorithm>
#include <fstream>
#include <sstream>
#include <windows.h>

#include "ChatExporterHtml.h"
#include "../Exceptions/Exception.h"
#include "../Libraries/Base64/Base64.h"
#include "../Libraries/UTF8/utf8.h"
#include "../Platforms/Win32/ImageDecoder.h"
#include "../Platforms/Win32/Resource.h"
#include "../Platforms/Win32/Timestamp.h"
#include "../Platforms/Win32/GUI/SmileyList.h"
#include "../Platforms/Win32/GUI/ChatControl/Elements/Messages/ChatControlMessageAudio.h"
#include "../WhatsApp/Chat.h"
#include "../WhatsApp/Emoticons.h"
#include "../WhatsApp/Message.h"
#include "../Settings.h"
#include "../../resources/resource.h"

ChatExporterHtml::ChatExporterHtml(Settings &settings, const std::string &templateHtml, const std::string &templateText, const std::string &templateAuthor, const std::string &templateImage)
	: settings(settings), templateHtml(templateHtml), templateText(templateText), templateAuthor(templateAuthor),
	templateImage(templateImage)
{
}

ChatExporterHtml::~ChatExporterHtml()
{
}

std::string ChatExporterHtml::buildMessages(WhatsappChat &chat, std::set<int> &usedEmoticons)
{
	std::stringstream output;

	std::vector<WhatsappMessage *> &messages = chat.getMessages(true);
	for (std::vector<WhatsappMessage *>::iterator it = messages.begin(); it != messages.end(); ++it)
	{
		WhatsappMessage &message = **it;

		switch (message.getMediaWhatsappType())
		{
			case MEDIA_WHATSAPP_TEXT:
			{
				if (message.getData().size() == 0)
					continue;

				std::string text = templateText;

				if (message.getRemoteResource().size() > 0)
				{
					std::string author = templateAuthor;
					replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
					replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
					replacePlaceholder(text, "%AUTHOR%", author);
				}
				else {
					replacePlaceholder(text, "%AUTHOR%", "");
				}
				replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
				replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
				replacePlaceholder(text, "%DATE%", "");
				replacePlaceholder(text, "%DATA_ID%", "");
				replacePlaceholder(text, "%MESSAGE%", convertMessageToHtml(message, usedEmoticons));
				output << text << std::endl;

			} break;
			case MEDIA_WHATSAPP_IMAGE:
			{
				if (message.getRawDataSize() > 0 && message.getRawData() != NULL)
				{
					std::string text = templateImage;
					if (message.getRemoteResource().size() > 0)
					{
						std::string author = templateAuthor;
						replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
						replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
						replacePlaceholder(text, "%AUTHOR%", author);
					}
					else {
						replacePlaceholder(text, "%AUTHOR%", "");
					}
					std::stringstream imagedata;
					imagedata << "data:image/jpeg;base64," << base64_encode(message.getRawData(), message.getRawDataSize());
					replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
					replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
					replacePlaceholder(text, "%DATE%", "");
					replacePlaceholder(text, "%DATA_ID%", "");
					replacePlaceholder(text, "%IMAGE_DATA%", imagedata.str());
					replacePlaceholder(text, "%MESSAGE%", convertMessageToHtml(message, usedEmoticons));
					output << text << std::endl;
				}
			} break;
			
			case MEDIA_WHATSAPP_AUDIO:
			{
				std::string text = templateText;

				if (message.getRemoteResource().size() > 0)
				{
					std::string author = templateAuthor;
					replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
					replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
					replacePlaceholder(text, "%AUTHOR%", author);
				}
				else {
					replacePlaceholder(text, "%AUTHOR%", "");
				}
				replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
				replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
				replacePlaceholder(text, "%DATE%", "");
				replacePlaceholder(text, "%DATA_ID%", "");
				replacePlaceholder(text, "%MESSAGE%", "[AUDIO]");
				output << text << std::endl;
			} break;
			case MEDIA_WHATSAPP_VIDEO:
			{
				if (message.getRawDataSize() > 0 && message.getRawData() != NULL)
				{
					std::string text = templateImage;
					if (message.getRemoteResource().size() > 0)
					{
						std::string author = templateAuthor;
						replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
						replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
						replacePlaceholder(text, "%AUTHOR%", author);
					}
					else {
						replacePlaceholder(text, "%AUTHOR%", "");
					}
					std::stringstream imagedata;
					imagedata << "data:image/jpeg;base64," << base64_encode(message.getRawData(), message.getRawDataSize());
					replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
					replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
					replacePlaceholder(text, "%DATE%", "");
					replacePlaceholder(text, "%DATA_ID%", "");
					replacePlaceholder(text, "%IMAGE_DATA%", imagedata.str());
					replacePlaceholder(text, "%MESSAGE%", "[VIDEO]");
					output << text << std::endl;
				}
			} break;
			case MEDIA_WHATSAPP_CONTACT:
			{
				std::string text = templateText;

				if (message.getRemoteResource().size() > 0)
				{
					std::string author = templateAuthor;
					replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
					replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
					replacePlaceholder(text, "%AUTHOR%", author);
				}
				else {
					replacePlaceholder(text, "%AUTHOR%", "");
				}
				replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
				replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
				replacePlaceholder(text, "%DATE%", "");
				replacePlaceholder(text, "%DATA_ID%", "");
				replacePlaceholder(text, "%MESSAGE%", "[CONTACT]");
				output << text << std::endl;
			} break;
			case MEDIA_WHATSAPP_LOCATION:
			{
				std::string text = templateText;
				std::stringstream data;
				data << "[ Location: " << message.getLatitude() << "; " << message.getLongitude() << " ]";
				if (message.getRemoteResource().size() > 0)
				{
					std::string author = templateAuthor;
					replacePlaceholder(templateAuthor, "%CONTACT_NAME%", settings.findDisplayName(message.getRemoteResource()));
					replacePlaceholder(templateAuthor, "@s.whatsapp.net", "");
					replacePlaceholder(text, "%AUTHOR%", author);
				}
				else {
					replacePlaceholder(text, "%AUTHOR%", "");
				}
				replacePlaceholder(text, "%INOUT%", message.isFromMe() ? "out" : "in");
				replacePlaceholder(text, "%TIME%", formatTimestamp(message.getTimestamp()));
				replacePlaceholder(text, "%DATE%", "");
				replacePlaceholder(text, "%DATA_ID%", "");
				replacePlaceholder(text, "%MESSAGE%", data.str());
				output << text << std::endl;
				
			} break;

		}

	}

	return output.str();
}

void ChatExporterHtml::replacePlaceholder(std::string &html, const std::string &placeholder, const std::string &text)
{
	int start = html.find(placeholder);
	while (start != std::string::npos) {
		html.replace(start, placeholder.length(), text);
		start = html.find(placeholder);
	}
}

std::string ChatExporterHtml::convertMessageToHtml(WhatsappMessage &message, std::set<int> &usedEmoticons)
{
	std::string messageString = message.getData();
	std::stringstream output;

	try
	{
		for (std::string::iterator it = messageString.begin(); it != messageString.end();)
		{
			std::string::iterator before = it;
			int character = utf8::next(it, messageString.end());

			if (isSmiley(character))
			{
				usedEmoticons.insert(character);
				output << "<span class=\"emoticon_" << std::hex << character << "\"></span>";
			}
			else
			{
				output << std::string(before, it);
			}
		}
	}
	catch (utf8::exception &exception)
	{
		output << "[INVALID DATA: " << messageString << "]";
	}

	return output.str();
}

std::string ChatExporterHtml::buildEmoticonStyles(const std::set<int> &usedEmoticons)
{
	std::stringstream css;
	for(std::set<int>::const_iterator it = usedEmoticons.begin(); it != usedEmoticons.end(); ++it)
	{
		int character = *it;
		int smileyIndex = -1;

		for (int i = 0; i < smileyCount; i++)
		{
			if (smileyList[i].character == character)
			{
				smileyIndex = i;
				break;
			}
		}

		if (smileyIndex >= 0)
		{
			unsigned char *bytes = NULL;
			DWORD size = 0;
			loadResource(MAKEINTRESOURCE(smileyList[smileyIndex].resource), L"PNG", bytes, size);
			std::string base64Emoticon = base64_encode(bytes, size);

			css << ".emoticon_" << std::hex << character << " {" << std::endl;
			css << "display: inline-block;" << std::endl;
			css << "width: 20px;" << std::endl;
			css << "height: 20px;" << std::endl;
			css << "background-image: url(data:image/png;base64," << base64Emoticon << ")" << std::endl;
			css << "}" << std::endl;
		}
	}

	return css.str();
}

void ChatExporterHtml::exportChat(WhatsappChat &chat, const std::string &filename)
{
	std::set<int> usedEmoticons;
	std::string messages = buildMessages(chat, usedEmoticons);

	std::string contact = chat.getKey();
	if (chat.getSubject().length() > 0)
	{
		contact = chat.getSubject();
	}

	std::string contactName = chat.getDisplayName();

	if (contactName == contact)
	{
		contactName = "";
	}

	std::ofstream file(filename.c_str());
	if (!file)
	{
		throw Exception("could not open chat export file");
	}

	std::string html = templateHtml;
	std::string heading = "WhatsApp Chat";
	replacePlaceholder(html, "%HEADING%", heading);
	replacePlaceholder(html, "%TITLE%", heading + " " + contact);
	replacePlaceholder(html, "%CONTACT%", contact);
	replacePlaceholder(html, "%CONTACT_NAME%", contactName);
	replacePlaceholder(html, "%MESSAGES%", messages);
	replacePlaceholder(html, "%EMOTICON_STYLES%", buildEmoticonStyles(usedEmoticons));

	file << html;
}
