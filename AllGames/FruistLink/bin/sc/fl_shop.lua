#define	初始化	oninit
#include "fl_base.lua"

fl_shop = nil;
function oninit()
	fl_shop = dlg_load("t_shop");
	fl_shop:enable("background", true);
	fl_shop:bind_click("close", "返回主页面");
	fl_shop:bind_click("买魔晶", "买魔晶");
	fl_shop:bind_click("去广告", "去广告");
end

function 买魔晶()
	播放音效("click");
end
function 去广告()
	播放音效("click");
end
