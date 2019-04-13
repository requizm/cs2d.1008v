#pragma once
#include "stdafx.h"
#include "glDraw.h"

float centerTextX(MyRect box, std::string text);

vec2 centerTextXY(MyRect box, std::string text);

class Menu
{
public:
	bool bStatus = true;

	std::string menuText[3];
	std::string sAimbotStatus = "OFF";
	std::string sESP = "OFF";
	std::string sFlashHack = "ON";

	float textHeight = 15;
	float buff = 2;
	float menuWidth = 0;
	float header = buff + textHeight + buff;
	float textWidth = 9;
	float buffW = textWidth * 2;
	float tabHeight = buff + textHeight + buff;
	float numFunctions = 8;
	float body = buff + numFunctions * (textHeight + buff) + buff;
	float footer = header;
	float outlineWidth = 2;

	//new
	void renderLine(std::string textString, float &xpos, float &ypos)
	{
		//font size?
		font.Print(xpos, ypos, rgb::red, "%s", textString.c_str());
		ypos += textHeight + buff;
		glRasterPos2f(xpos, ypos);
	}

	void DrawMenu()
	{
		std::vector <std::string> functions;
		std::string sHeader = "CS2D Hack -requizm";
		functions.push_back("[F5] Aimbot    : " + sAimbotStatus);
		functions.push_back("[F6] ESP       : " + sESP);
		functions.push_back("Flash Hack(Unalterable) : " + sFlashHack);
		functions.push_back("[HOME] Hide Menu");

		//Get Menu width
		std::string longest = functions[0];
		for each (std::string s in functions)
		{
			if (s.length() > longest.length())
			{
				longest = s;
			}
		}

		if (sHeader.length() >= longest.length())
		{
			menuWidth = (sHeader.length() * textWidth) + buffW;
		}
		else menuWidth = (longest.length() * textWidth) + buffW;

		MyRect header1;
		/*header1.tl = { float(viewport[2] * .65 - menuWidth / 2),float(viewport[3] * .01) };
		header1.tr = { float(viewport[2] * .65 + menuWidth / 2), float(viewport[3] * .01) };
		header1.br = { float(viewport[2] * .65 + menuWidth / 2), float(viewport[3] * .01 + header) };
		header1.bl = { float(viewport[2] * .65 - menuWidth / 2), float(viewport[3] * .01 + header) };*/
		header1.tl = { 0,0 };
		header1.tr = { menuWidth, 0 };
		header1.br = { menuWidth, tabHeight };
		header1.bl = { 0, tabHeight };
		float middlex = header1.tl.x + ((header1.tr.x - header1.tl.x) / 2);
		float left = header1.tl.x;
		float right = header1.tr.x;

		glDisable(GL_TEXTURE_2D);
		glColor3ub(55, 55, 55);//gray
		drawFilledRect(header1);
		glEnable(GL_TEXTURE_2D);

		glDisable(GL_TEXTURE_2D);
		glColor3ub(0, 0, 0); //black
		drawOutline(header1);
		glEnable(GL_TEXTURE_2D);

		//draw text
		vec2 pos = centerTextXY(header1, sHeader);
		glDisable(GL_TEXTURE_2D);
		glColor3ub(255, 0, 0);
		glRasterPos2f(pos.x, pos.y);
		renderText(sHeader, pos.x, pos.y);
		glEnable(GL_TEXTURE_2D);

		MyRect body1;
		body1.tl = { left, tabHeight + outlineWidth };
		body1.tr = { right, tabHeight + outlineWidth };
		body1.br = { right, tabHeight + outlineWidth + body - 50 };
		body1.bl = { left, tabHeight + outlineWidth + body - 50 };

		glDisable(GL_TEXTURE_2D);
		glColor3ub(55, 55, 55);//gray
		drawFilledRect(body1);
		glEnable(GL_TEXTURE_2D);

		glDisable(GL_TEXTURE_2D);
		glColor3ub(0, 0, 0); //black
		drawOutline(body1);
		glEnable(GL_TEXTURE_2D);

		//Body Text writing section:
		float textrasterHeight = body1.tl.y + buff + textHeight;
		float textrasterWidth = centerTextX(body1, longest);

		glDisable(GL_TEXTURE_2D);
		glColor3ub(255, 0, 0);
		glRasterPos2f(textrasterWidth, textrasterHeight);

		for each (std::string string in functions)
		{
			renderLine(string, textrasterWidth, textrasterHeight); //15 font
		}
		glEnable(GL_TEXTURE_2D);

		MyRect footer1;
		footer1.tl = { left, body1.bl.y + outlineWidth };
		footer1.tr = { right, body1.br.y + outlineWidth };
		footer1.br = { right, body1.br.y + outlineWidth + footer };
		footer1.bl = { left, body1.bl.y + outlineWidth + footer };

		glDisable(GL_TEXTURE_2D);
		glColor3ub(55, 55, 55);//gray
		drawFilledRect(footer1);
		glEnable(GL_TEXTURE_2D);

		glDisable(GL_TEXTURE_2D);
		glColor3ub(0, 0, 0); //black
		drawOutline(footer1);
		glEnable(GL_TEXTURE_2D);
	}
}menu;

//defs
float centerTextX(MyRect box, std::string text)
{
	float boxWidth = box.tr.x - box.tl.x;
	float textWidth = text.length() * 9;
	float difference = boxWidth - textWidth;
	return (box.tl.x + (difference / 2));
}

vec2 centerTextXY(MyRect box, std::string text)
{
	vec2 textPOS;
	float boxWidth = box.tr.x - box.tl.x;
	float textWidth = text.length() * 9;
	float differenceX = boxWidth - textWidth;
	textPOS.x = box.tl.x + (differenceX / 2);

	float boxHeight = box.bl.y - box.tl.y;
	float textHeight = 15;
	float differenceY = boxHeight - textHeight;
	textPOS.y = box.tl.y + textHeight - (differenceY / 4);//why??
	return textPOS;
}