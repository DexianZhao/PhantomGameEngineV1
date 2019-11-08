#define	初始化	oninit
#include "fl_base.lua"
shuangjieMJ = 1000;
quanjieMJ = 1000;
fl_main = nil;
function 更新钻石显示()
	初始化数据();
end
function oninit()
	setlanguage(readt("languages", "cn"));--初始化地区语言
	msgbox = dlg_search("msgbox");
	msgbox.display = false;
	fl_main = dlg_load("t_main");
	fl_main.pos = Pixel(0,0);
	fl_main:enable("background", true);
	fl_main:bind_click("进入简单", "进入冒险模式");
	fl_main:bind_click("进入挑战", "进入挑战模式");
	fl_main:bind_click("更多游戏", "进入更多游戏");
	fl_main:bind_click("购买钻石", "进入购买页面");
	fl_main:bind_click("评论", "进入评论");
	fl_main:bind_click("排行榜", "进入排行榜");
	fl_main:bind_click("免费领钻石", "进入领钻石");
	fl_main:bind_click("中文", "设置为中文");
	fl_main:bind_click("英文", "设置为英文");
	fl_main:show("免费领钻石", false);
	if readi("gamefirst",0) == 0 then
		writei("magic", 100);--初始法力值100
		writei("mojing", 0);
		writei("gamefirst",1);
		writei("ok_1", 1);
		local thistime = localtime();
		writei("fday", thistime.day);
		print("day:"..thistime.day);
	else
		--local thistime = math.abs(localtime().day-readi("fday", 0));
		--if thistime > 1 then
			--fl_main:show("免费领钻石", true);
		--end
		--print("day:"..localtime().day);
	end
	if insystem("android") then
		fl_main:show("免费领钻石", true);
	end
	初始化数据();
end

function 设置为中文()
	
	播放音效("reset");
	writet("languages", "cn");
	goto("fl_main");
end
function 设置为英文()
	播放音效("reset");
	writet("languages", "en");
	goto("fl_main");
end
function 初始化数据()
	setvali("lianxu", 0);
	setvali("newrecord", 0);
	setvali("jifen", 0);
	local maxlx = readi("lianxu", 0);
	local maxstar = MAX_MISSIONS*3;
	local stars = 0;
	for i=0,(MAX_MISSIONS-1) do
		local s = readi("ok_"..i, 0);
		if s > 0 then
		stars = stars + (s-1);
		end
	end
	fl_main:setvalt("当前关卡值", readi("current"));
	fl_main:setvalt("完成度值", int(stars/maxstar*100).."%");
	fl_main:setvali("最高连续过关值", maxlx);
	fl_main:setvali("最高积分纪录值", readi("newrecord", 0));
	local bwuxian = (readi("current")>20);
	fl_main:enable("连续模式", bwuxian);
	fl_main:show("无限开启", bwuxian==false);
	local besttime = readi("besttime", 0);
	local minute = int(besttime/60);
	local sec = int(besttime%60);
	if sec < 10 then
		sec = "0"..sec;
	end
	if minute < 10 then
		minute = "0"..minute;
	end
	fl_main:setvalt("最快时间比", minute..":"..sec);
	setvali("reset", 1);
	local mojing = readi("mojing", 0);
	fl_main:setvali("当前钻石", mojing);
end
function 成功购买()
end
function 进入购买页面()
	播放音效("click");
	shop_buy("mjlink.buygem", "成功购买");
end
function 进入冒险模式()
	播放音效("click");
	setvali("current", readi("current"));
	goto("fl_point");
end
function 进入挑战模式()
	播放音效("click");
	setvali("beginsj", 10000);
	setvali("current", 10000);
	goto("fl_game");
end

function 进入更多游戏()
	播放音效("click");
	openurl("http:/\/www.aixspace.com/app");
end

function 进入评论()
	播放音效("click");
	gotocomment();
end

function 进入排行榜()
	local maxstar = MAX_MISSIONS*3;
	local stars = 0;
	for i=0,(MAX_MISSIONS-1) do
		local s = readi("ok_"..i, 0);
		if s > 0 then
		stars = stars + (s-1);
		end
	end
	播放音效("click");
	achievement("mjlink.missioncomplete", int(stars/maxstar*100));
	rankscore("mjlink.maxscore", readi("newrecord", 0));
	rankscore("mjlink.maxmission", readi("lianxu", 0));
	openrank("");--显示排行榜
end

