#define	初始化	oninit
#include "fl_base.lua"
fl_game = nil;
majiangs = {};
majiangs_sel = {};
majiangs_pos = {};
majiangs_exist = {};
majiangs_disable = {};
majiangs_count = 0;
majiangs_double = 41;
majiangs_zubie = 2;
fl_region = nil;--(UIControl)
fl_effect = {};
fl_effect_play = {};
fl_effect_count = 6;
crystal_item = {};
crystal_scale = 0.5;
game_mj = {};
game_undos = {};
game_undo_count = 0;
game_mj_new = {};
game_mjcount_w = 6;
game_mjcount_h = 6;
mjwidth = 55;
mjheight = 70;
effect_mj_offset = 10;
game_time_enable = true;
game_begin = false;
game_mj_beginX = 0;
game_mj_beginY = 0;
game_mj_index = {};
game_mj_count = 0;
game_mj_exists = 0;
mojing_offset = 50;
mj_min_offset = 10;--麻將最少移動像素
g_mousedisable = false;
mj_effects = {};
mj_effect_count = 0;
game_rect = nil;
钻石出现概率 = 0;
处于置换状态 = false;
g_movedisable = false;
chongzhi_guan = 16;
jiashi_guan = 24;
xiaochu_guan = 8;
help_array = {1,2,8,10,16,21,24,31};
help_array_name = {"xiaochu","tuitui","tishi","mofa", "chexiao","wuxuxiaochu","chuansong","zudang"};
xiaochu_eff = {};
xiaochu_pos = {};
xiaochu_pos_count = {};
xiaochu_index = 0;
function oninit()
	config = table_load("config.txt");
	fl_game = dlg_load("t_game");
	fl_game.pos = Pixel(0,0);
	fl_game:enable("background", true);
	--fl_game:bind_click("close", "关闭并能返回关卡");
	fl_game:bind_click("pause", "暂停游戏");
	--fl_game:bind_click("help", "显示帮助");
	fl_region = fl_game:search("region");
	
	for i=0,(fl_effect_count-1) do
		fl_effect[i] = fl_game:search("effect"..(i+1));
	end
	xiaochu_eff[0] = fl_game:search("effect11");
	xiaochu_eff[1] = fl_game:search("effect12");
	xiaochu_eff[2] = fl_game:search("effect13");

	bind_input("down", "手指按下");
	bind_input("up", "手指弹起");
	bind_input("move", "手指移动");
	bind_input("dbldown", "双手指按下");
	bind_input("dblup", "双手指弹起");
	bind_input("dblmove", "双手指移动");
	game_mjcount_w = config:id_i(getvali("current"), "宽数量");
	game_mjcount_h = config:id_i(getvali("current"), "高数量");
	majiangs_zubie = config:id_i(getvali("current"), "组别");
	mjcount_tbl = config:id_i(getvali("current"), "数量");
	mjsize_tbl = config:id_i(getvali("current"), "麻将大小") * 55 / 100;
	ResetGame();
	g_savecombocount	=	0;
	--
	for i=0,(fl_effect_count-1) do
		crystal_item[i] = draw_create();
		crystal_item[i].context = 0;
		crystal_item[i].center = Pixel(128,128);
		fl_region:bind_draw(crystal_item[i]);
		crystal_item[i]:image("images/crystal.png");
		crystal_item[i].pos = Pixel(0, 0);
		crystal_item[i].scale = crystal_scale;
		crystal_item[i].clickscale = 1;
		crystal_item[i].display = false;
		crystal_item[i].layer = 101;
	end
end
move_crystal_dir = {};
move_crystal_begin = {};
function 设置特效位置(eff, mj)
	local pos = Pixel(game_mj_beginX+mj.x * mjwidth+45, game_mj_beginY+mj.y * mjheight+50);
	eff.display = true;
	pos.x = pos.x + fl_region.left;
	pos.y = pos.y + fl_region.top;
	eff.pos = pos;
	eff.effect_stop = true;
	eff.action = "Track";

end
function 显示帮助()
	local current = getvali("current");
	local name = help_array_name[table.getn(help_array)];
	for x=1,(table.getn(help_array)-1) do
		print("TLL"..help_array[x]);
		if current < help_array[x+1] then
			name = help_array_name[x];
			break;
		end
	end
	显示帮助页面(name);
end
function 进入老陈杂货()
	播放音效("click");
	timeout2("进入下一个剧情", 500, "fl_shop");
end
function 显示帮助页面(name)
	-- cleartime("时间减少即时");
	-- if fl_help == nil then
		-- fl_help = dlg_load("help");
	-- end
	-- cleartime("时间减少即时");
	-- 弹出对话框("help");
	-- fl_help.domodal = true;
	-- fl_help:bind_click("close", "关闭帮助");
	-- fl_help:bind_click("image", "关闭帮助");
	-- fl_help.display = true;
	-- fl_help:show("领取奖励", false);
	-- fl_help:show("老陈杂货", false);
	-- fl_help:bind_image("image", "help/"..name..".png");
end
function 关闭帮助()
	继续游戏();
	if fl_help ~= nil then
		dlg_delete(fl_help);
		fl_help = nil;
	end
end
function 刷新工具使用()
	local current = readi("current");
	if getvali("current") > current then
		current = getvali("current");
	end
	local xc =config:id_i(getvali("current"), "加时钻石");
	local cz =config:id_i(getvali("current"), "重置钻石");
	local find =config:id_i(getvali("current"), "消除钻石");
	local mj = readi("mojing");
	fl_game:enable("加时按钮", game_time_enable);--xc<=mj and game_time_enable);
	fl_game:enable("重置按钮", true);--cz<=mj);
	fl_game:enable("消除按钮", true);--find<=mj);
	fl_game:enable("加时说明", game_time_enable);--xc<=mj and game_time_enable);
	fl_game:enable("重置说明", true);--cz<=mj);
	fl_game:enable("消除说明", true);--find<=mj);
	fl_game:enable("加时值", game_time_enable);--xc<=mj and game_time_enable);
	fl_game:enable("重置值", true);--cz<=mj);
	fl_game:enable("消除值", true);--find<=mj);

	fl_game:enable("购买按钮", true);
	fl_game:bind_click("加时按钮", "增加时间功能");
	fl_game:bind_click("重置按钮", "重置游戏功能");
	fl_game:bind_click("消除按钮", "随机查找一个");
	fl_game:bind_click("购买按钮", "进入购买页面");
	fl_game:show("加时按钮", current >= jiashi_guan);
	fl_game:show("消除按钮", current >= xiaochu_guan);
	fl_game:show("重置按钮", current >= chongzhi_guan);
	fl_game:show("加时说明", current >= jiashi_guan);
	fl_game:show("消除说明", current >= xiaochu_guan);
	fl_game:show("重置说明", current >= chongzhi_guan);
	fl_game:show("加时值", current >= jiashi_guan);
	fl_game:show("消除值", current >= xiaochu_guan);
	fl_game:show("重置值", current >= chongzhi_guan);
	fl_game:setvali("加时值", xc);
	fl_game:setvali("消除值", find);
	fl_game:setvali("重置值", cz);
end

fl_help = nil;

function 开始时显示帮助()
	local current = getvali("current");
	local name = nil;
	for x=1,table.getn(help_array) do
		--print("TLL"..help_array[x]);
		if current == help_array[x] then
			name = help_array_name[x];
			break;
		end
	end
	if current >= 10000 and current < 11000 then
		name = "zhijiaoxiaochu";
		current = 10000;
	end
	if name == nil then
		return;
	end
	-- if readi("help"..current, 0) == 0 then
		-- writei("help"..current, 1);
	-- else
		-- return;
	-- end
	显示帮助页面(name);
end
function 返回线上可消除(mjpos)
	local mjindex = game_mj[mjpos.x][mjpos.y];
	if majiangs_disable[mjindex] ~= 0 then
		return nil;
	end
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			local index = game_mj[x][y];
			if index >= 0 then
				if 是否可消除(mjindex,index, false)==true then
					return Pixel(x,y);
				end
			end
		end
	end
	return nil;
end
function 替换一个麻将()
end

可消除位置1 = nil;
可消除位置2 = nil;
function 是否可以随机消除()
	可消除位置1 = nil;
	可消除位置2 = nil;
	local bok = false;
	local cnt = 0;
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			if game_mj[x][y] >= 0 then
				cnt = cnt+1;
			end
		end
	end
	if cnt == 0 then
		return true;
	end
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			if game_mj[x][y] >= 0 and majiangs_disable[game_mj[x][y]] == 0 then
				local ret = 返回线上可消除(Pixel(x,y));
				if ret ~= nil then
					可消除位置1 = Pixel(x,y);
					可消除位置2 = ret;
					bok = true;
					break;
				end
			end
		end
		if bok then
			break;
		end
	end
	if 可消除位置1 ~= nil then
		return true;
	end
	return false;
end
function 重置游戏功能()
	local mj = readi("mojing");
	if mj < config:id_i(getvali("current"), "重置钻石") then
		钻石不足();
		播放音效("de");
		return;
	end
	fl_game:enable("重置按钮", false);
	timeout("刷新工具使用", 1000);
	if 扣除魔晶(config:id_i(getvali("current"), "重置钻石"))==false then
		钻石不足();
		播放音效("de");
		return;
	end
	重置游戏并回到游戏();
end

function 增加时间功能()
	local mj = readi("mojing");
	if mj < config:id_i(getvali("current"), "加时钻石") then
		钻石不足();
		播放音效("de");
		return;
	end
	fl_game:enable("加时按钮", false);
	timeout("刷新工具使用", 1000);
	if 扣除魔晶(config:id_i(getvali("current"), "加时钻石"))==false then
		钻石不足();
		播放音效("de");
		return;
	end
	播放音效("addtime");
	game_time = game_time + 10;
	if game_time > game_time_length then
		game_time = game_time_length;
	end
	game_time_enable	=	false;
	timeout("刷新时间使用", 10000);
end
function 刷新时间使用()
	game_time_enable = true;
	刷新工具使用();
end
function 随机查找一个()
	local mj = readi("mojing");
	if mj < config:id_i(getvali("current"), "消除钻石") then
		钻石不足();
		播放音效("de");
		return;
	end
	fl_game:enable("消除按钮", false);
	timeout("刷新工具使用", 1000);
	if 是否可以随机消除() then
		if 扣除魔晶(config:id_i(getvali("current"), "消除钻石"))==false then
			钻石不足();
			播放音效("de");
			return;
		end
		majiangs_sel[0].pos = Pixel(game_mj_beginX + 可消除位置1.x * mjwidth, game_mj_beginY + 可消除位置1.y * mjheight);
		majiangs_sel[1].pos = Pixel(game_mj_beginX + 可消除位置2.x * mjwidth, game_mj_beginY + 可消除位置2.y * mjheight);
		majiangs_sel[0].display = true;
		majiangs_sel[1].display = true;
	else
		播放音效("warning");
	end
	--打印gamemj();
end
function 进入购买页面()
	播放音效("click");
	shop_buy("mjlink.buygem", "成功购买");
end
function 更新钻石显示()
	fl_game:setvali("当前钻石", readi("mojing"));
end
function 钻石不足()
	显示游戏结束框();
	local 钻石不足 = fl_game_over:gsearch("钻石不足");
	钻石不足.display = true;
	钻石不足:bind_click("购买按钮", "进入购买页面");
	钻石不足:show("免费领钻石", false);
	--钻石不足:bind_click("免费领钻石", "进入领钻石");
	--local thistime = math.abs(localtime().day-readi("fday", 0));
	--if thistime > 1 then
		--钻石不足:show("免费领钻石", true);
		--钻石不足:bind_click("免费领钻石", "进入领钻石");
	--end
	if insystem("android") then
		钻石不足:show("免费领钻石", true);
		钻石不足:bind_click("免费领钻石", "进入领钻石");
	end
	钻石不足:bind_click("继续", "继续游戏");
end
function 重置游戏()
	显示游戏结束框(false);
	fl_game_over:show("过关失败", false);
	fl_game_over:show("重置游戏", true);
	fl_game_over:bind_click("close", "关闭重置界面");
	fl_game_over:bind_click("重来", "重置游戏并回到游戏");
	
end
function 隐藏控件(c)
	c.display = false;
end
function 扣除魔晶(n)
	local mj = readi("mojing");
	if mj < n then
		return false;
	end
	fl_game:setvalt("当前钻石减", "-"..n)
	fl_game:show("当前钻石减", true);
	fl_game:search("当前钻石减").action = "aniout";
	fl_game:bind_event("onplayover", "当前钻石减", "隐藏控件");
	writei("mojing", mj - n);
	fl_game:setvali("当前钻石", readi("mojing"));
	return true;
end
function 关闭重置界面()
	if fl_game_over ~= nil then
		ad_show(1, false);
		dlg_delete(fl_game_over);
		fl_game_over = nil;
	end
end
function 游戏重来()
	 if fl_game_over ~= nil then
		ad_show(1, false);
		 dlg_delete(fl_game_over);
		 fl_game_over = nil;
		 播放音效("begin");
		game_mjcount_w = config:id_i(getvali("current"), "宽数量");
		game_mjcount_h = config:id_i(getvali("current"), "高数量");
		majiangs_zubie = config:id_i(getvali("current"), "组别");
		mjcount_tbl = config:id_i(getvali("current"), "数量");
		mjsize_tbl = config:id_i(getvali("current"), "麻将大小") * 55 / 100;
		ResetGame();
	 end
end
function 重置游戏并回到游戏()
	播放音效("disable");
	local ary = {};
	local index = 0;
	for i=0,(game_mjcount_w-1) do
		for j=0,(game_mjcount_h-1) do
			local mjindex = game_mj[i][j];
			if mjindex >= 0 and majiangs_disable[mjindex] == 0 then
				ary[index] = {};
				ary[index][0] = i;
				ary[index][1] = j;
				ary[index][2] = mjindex;
				index = index + 1;
			end
		end
	end
	-- for i=0,(game_mjcount_w-1) do
		-- for j=0,(game_mjcount_h-1) do
			-- game_mj[i][j] = -1;
		-- end
	-- end
	random_table(index);
	for i=0,(index-1) do
		local idx = random_get(i);
		game_mj[ary[i][0]][ary[i][1]] = ary[idx][2];
	end
	for i=0,(fl_effect_count-1) do
		fl_effect_play[i] = 0;
	end
	index = 0;
	for i=0,(game_mjcount_w-1) do
		for j=0,(game_mjcount_h-1) do
			local mjindex = game_mj[i][j];
			if mjindex >= 0 then
				majiangs[mjindex].pos = Pixel(i*mjwidth+game_mj_beginX, j*mjheight+game_mj_beginY);
				timeout2("麻将渐显出来", index*50, mjindex);
				index = index + 1;
			end
		end
	end
	按照麻将位置重新计算GameMJ();
	 if fl_game_over ~= nil then
		 ad_show(1, false);
		 dlg_delete(fl_game_over);
		 fl_game_over = nil;
		 播放音效("begin");
		-- ResetGame();
	 end
	if lastsel_mj~= nil then
		last_index = game_mj[lastsel_mj.x][lastsel_mj.y];
		majiangs[last_index].color = color4(1,1,1,1);
		lastsel_mj = nil;
	end
	timeout("停止粒子播放", 1000);
	timeout("结束粒子播放", 1500);
end
mojing_index = 0;
move_last_begin = 0;
function 出现钻石(posin)
	local pos = Pixel(posin.x,posin.y);
	fl_game:setvali("当前钻石", readi("mojing"));
	--cleartime("移动钻石到下方");
	--cleartime("慢慢放到钻石框");
	mojing_index = mojing_index + 1;
	local sindex = mojing_index % fl_effect_count;
	writei("mojing", readi("mojing")+1);
	timeout("刷新工具使用", 3000);
	crystal_item[sindex].scale = crystal_scale;
	crystal_item[sindex].display = true;
	pos.x = pos.x - fl_region.left - mojing_offset - 70;
	pos.y = pos.y - fl_region.top - mojing_offset - 85;
	crystal_item[sindex].pos = pos;
	move_crystal_begin[sindex] = crystal_item[sindex].pos;
	local mj = fl_game:search("购买按钮");
	move_crystal_dir[sindex] = Pixel(mj.left - fl_region.left - mojing_offset, mj.top - fl_region.top - mojing_offset - 20) - move_crystal_begin[sindex];
	--fl_effect[sindex].display = true;
	--fl_effect[sindex].effect_loop = 4;
	move_last_pos = Pixel(pos.x + 96 + fl_region.left,pos.y + 96 + fl_region.top);
	move_last_begin = 0;	
	interval2("慢慢放到钻石框", 50, 1000, sindex);
end
function 慢慢放到钻石框(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	local index = atoi(param);
	local s = math.sin(tm*12+1) + crystal_scale;
	if s < crystal_scale then
		s = crystal_scale;
	end
	crystal_item[index].scale = s;
	if tm > 0.35 then
		crystal_item[index].scale = crystal_scale;
		breaktime();
		interval2("移动钻石到下方", 50, 2000, index);
	end
end

function 移动钻石到下方(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	local index = atoi(param);
	local newposx = move_crystal_begin[index].x + move_crystal_dir[index].x * tm;
	local newposy = move_crystal_begin[index].y + move_crystal_dir[index].y * tm;
	if move_last_begin == 0 then
		-- fl_effect[index].width = 64;
		-- fl_effect[index].height = 64;
		-- fl_effect[index].effect_stop = true;
		-- fl_effect[index].effect_loop = 5;
		-- fl_effect[index].action = "Track";
		-- move_last_begin = 1;
	end
	crystal_item[index].pos = Pixel(newposx, newposy);
	--fl_effect[index].moveleft = move_last_pos.x;
	--fl_effect[index].movetop = move_last_pos.y;
	move_last_pos = Pixel(newposx + 96 + fl_region.left,newposy + 96 + fl_region.top);
	if tm > 1 then
		breaktime();
		fl_effect[index].effect_stop = true;
		crystal_item[index].display = false;
		crystal_item[index].scale = 0.5;
		fl_game:setvali("当前钻石", readi("mojing"));
		fl_game:setvalt("当前钻石减", "+1")
		fl_game:show("当前钻石减", true);
		fl_game:search("当前钻石减").action = "ani";
		fl_game:bind_event("onplayover", "当前钻石减", "隐藏控件");
	end
end


function 时间减少即时(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	game_time = game_time - 1;
	local star2 = config:id_i(getvali("current"), "2星时间");
	if game_time < 0 then
		breaktime();
		g_mousedisable = true;
		local curr = getvali("current");
		if curr >= 10000 and curr < 11000 then
			timeout("爽解过关结束", 500);
		else
			timeout("游戏过关失败", 500);
		end
		setvali("reset", 1);
		播放消失特效(fl_game:search("star3"));
		fl_game:enable("star1", false);
		fl_game:enable("star2", false);
		fl_game:enable("star3", false);
		return;
	elseif game_time < 10 and game_time < star2 then
		播放音效("lowtime");
	end
	更新时间计数(game_time);
end
function 播放消失特效(c)
	local stareff = fl_game:search("stareff");
	stareff.action = "ani";
	stareff.display = true;
	stareff.left = c.left;
	stareff.top = c.top;
	stareff.width=38;
	stareff.height = 38;
	stareff:bind_event("onplayover", "隐藏控件");
end
function 更新时间计数(tm)
	-- local hour = int(tm/3600);
	-- local minute = int((tm%3600)/60);
	-- local second = int(tm%60);
	-- if second < 10 then
		-- second = "0"..second;
	-- end
	-- if minute < 10 then
		-- minute = "0"..minute;
	-- end
	-- if hour < 10 then
		-- hour = "0"..hour;
	-- end
	-- local endtime = localtime();
	-- local second = timediff_s(endtime, begin_time) - g_timeadd;
	-- tm = game_time_length - second;
	-- ppk_game:setvalt("时间值", hour..":"..minute..":"..second);
	local progress = tm / game_time_length;
	local 时间 = fl_game:search("时间");
	时间.right = 960*progress;
	local star1 = config:id_i(getvali("current"), "时间限制");
	local star2 = config:id_i(getvali("current"), "2星时间");
	local star3 = config:id_i(getvali("current"), "3星时间");
	--local endtime = localtime();
	local second = game_time_length - tm;--timediff_s(endtime, begin_time) - g_timeadd;
	local text = "x3";
	if getvali("current") >= 10000 and getvali("current")<11000 then
		return;
	end
	fl_game:enable("star1", true);
	fl_game:enable("star2", true);
	fl_game:enable("star3", true);
	fl_game:show("star1", true);
	fl_game:show("star2", true);
	fl_game:show("star3", true);
	if tm>=star2 and tm < star3 then
		--print("gametime:"..game_time_length..","..tm..","..star3..","..second);
		if g_laststarcount ~= 3 then
			播放消失特效(fl_game:search("star1"));
			g_laststarcount = 3;
			播放音效("lose");
		end
		fl_game:enable("star1", false);
	elseif tm < star2 then
		if g_laststarcount ~= 2 then
			播放消失特效(fl_game:search("star2"));
			g_laststarcount = 2;
			播放音效("lose");
		end
		fl_game:enable("star1", false);
		fl_game:enable("star2", false);
	end
end
function 关闭并能返回关卡()
	播放音效("click");
	local oldi = getvali("current");
	if oldi >= 10000 then
		goto("main");
	else
		goto("fl_point");
	end
end
begin_time = nil;
最大魔晶出现数量 = 0;
function 暂停游戏()
	播放音效("click");
	显示游戏结束框();
	local 暂停游戏 = fl_game_over:gsearch("暂停游戏");
	暂停游戏.display = true;
	暂停游戏:bind_click("退出", "关闭并能返回关卡");
	暂停游戏:bind_click("继续", "继续游戏");
	暂停游戏:bind_click("购买按钮", "进入购买页面");
	暂停游戏:show("免费领钻石", false);
	--local thistime = math.abs(localtime().day-readi("fday", 0));
	--if thistime > 1 then
		--暂停游戏:show("免费领钻石", true);
		--暂停游戏:bind_click("免费领钻石", "进入领钻石");
	--end
	if insystem("android") then
		暂停游戏:show("免费领钻石", true);
		暂停游戏:bind_click("免费领钻石", "进入领钻石");
	end

	fl_game:show("region", false);
	fl_game:show("regionbg", false);
end
function 继续游戏()
	播放音效("click");
	interval("时间减少即时", 1000, game_time*2*1000);
	fl_game:show("region", true);
	fl_game:show("regionbg", true);
	if fl_game_over ~= nil then
		ad_show(1, false);
		dlg_delete(fl_game_over);
		fl_game_over = nil;
	end
end
g_combocount = 0;
g_combocountt = 0;
g_combocountg = 0;
g_combotime = 0;
g_laststarcount = 0;
function ResetGame()
	fl_game:show("连击加时", false);
	g_combocount = 0;
	g_combocountt=0;
	last_index = -1;
	lastsel_mj = nil;
	g_combocountg=0;
	g_timeadd = 0;
	fl_game:show("region", true);
	fl_game:show("regionbg", true);
	fl_game:show("stareff", false);
	更新连击();
	g_combotime = apptime();
	g_movedisable = false;
	begin_time = localtime();
	game_undos = {};
	game_undo_count = 0;
	处于置换状态 = false;
	mouse_down_mj = nil;
	fl_game:show("star1", true);
	fl_game:show("star2", true);
	fl_game:show("star3", true);
	g_lianji_c2 = fl_game:search("连击背景");
	g_lianji_c = fl_game:search("连击值");
	g_lianji_time = config:id_f(getvali("current"), "连击时间");
	g_lianji_c.display = false;
	g_lianji_c2.display = false;
	print("GAME_INDEX:"..getvali("current"));
	mjwidth = mjsize_tbl;
	mjheight = mjwidth/55*70;
	钻石出现概率 = config:id_i(getvali("current"), "钻石出现概率") / 100;
	最大魔晶出现数量 = config:id_i(getvali("current"), "最大钻石出现数量");
	if getvali("reset")== 1 or (getvali("current") < 10000 or getvali("current") >= 11000) then
		game_time = config:id_i(getvali("current"), "时间限制");
		game_time_length = game_time;
		setvali("reset", 0);
	end
	for i=0, (game_mj_count-1) do
		if game_mj_index[i] >= 0 then
			majiangs[game_mj_index[i]].display = false;
		end
	end
	更新时间计数(game_time);
	interval("时间减少即时", 1000, game_time*2*1000);
	mj_chanchu=0;
	if getvali("current") < 10000 then
		mj_chanchu = readi("mj_chanchu"..getvali("current"));
	end
	fl_game:setvalt("钻石值", mj_chanchu.."/"..最大魔晶出现数量);
	g_laststarcount = 4;
	local curI = getvali("current");
	fl_game:show("关卡", false);
	fl_game:show("连续关卡", false);
	fl_game:show("关卡值", false);
	fl_game:show("连续关卡值", false);
	fl_game:show("star1", curI<10000);
	fl_game:show("star2", curI<10000);
	fl_game:show("star3", curI<10000);
	fl_game:image_mode("star1", "ystar");
	fl_game:image_mode("star2", "ystar");
	fl_game:image_mode("star3", "ystar");
	fl_game:show("获得积分", curI>=10000);
	fl_game:show("积分值", curI>=10000);
	fl_game:setvalt("积分值", "x"..getvali("jifen"));
	if curI >= 10000 and curI < 11000 then
		fl_game:show("连续关卡", true);
		fl_game:show("连续关卡值", true);
		print("LIANXU:"..getvali("lianxu"));
		fl_game:setvali("连续关卡值", getvali("lianxu") + 1);

	elseif curI<10000 then
		fl_game:show("关卡", true);
		fl_game:show("关卡值", true);
		fl_game:setvali("关卡值", getvali("current"));
	else

	end
	--fl_game:setvali("法力值", readi("magic", 0));
	fl_game:setvali("当前钻石", readi("mojing"));
	fl_game:show("当前钻石减", false);
	local index = 0;
	majiangs_double = 0;
	for i=0,41 do
		majiangs_double = majiangs_double + 1;
		local name;
		if i <= 8 then
			name = "b_0"..(i+1);
		elseif i <= 17 then
			name = "t_0"..(i-8);
		elseif i <= 26 then
			name = "w_0"..(i-17);
		elseif i == 27 then
			name = "dong";
		elseif i == 28 then
			name = "xi";
		elseif i == 29 then
			name = "nan";
		elseif i == 30 then
			name = "bei";
		elseif i == 31 then
			name = "zhong";
		elseif i == 32 then
			name = "fa";
		elseif i == 33 then
			name = "bai";
		elseif i == 34 then
			name = "hua_chun";
		elseif i == 35 then
			name = "hua_xia";
		elseif i == 36 then
			name = "hua_qiu";
		elseif i == 37 then
			name = "hua_dong";
		elseif i == 38 then
			name = "hua_mei";
		elseif i == 39 then
			name = "hua_lan";
		elseif i == 40 then
			name = "hua_zhu";
		elseif i == 41 then
			name = "hua_ju";
		end
		for j=0,(majiangs_zubie-1) do
			if majiangs[index] == nil then
				majiangs[index] = draw_create();
				majiangs[index].context = index;
				majiangs[index].center = Pixel(22,25);
				fl_region:bind_draw(majiangs[index]);
			end
			majiangs[index]:image("images/mj.png");
			majiangs[index]:image_mode(name);
			majiangs[index]:set_rect(0,0,mjwidth/55*54,mjheight/70*78);
			majiangs[index].pos = Pixel((index%12)*70+j*24, 20 + int(index/12) * 26);
			majiangs[index].clickscale = 1.1;
			--majiangs[index]:bind_click("选择一个麻将");
			index = index + 1;
		end
	end
	for i=0,1 do
		if majiangs_sel[i] == nil then
			majiangs_sel[i] = draw_create();
			majiangs_sel[i].context = i;
			majiangs_sel[i].center = Pixel(22,25);
			fl_region:bind_draw(majiangs_sel[i]);
			majiangs_sel[i]:image("images/mj.png");
			majiangs_sel[i]:image_mode("sel");
		end
		majiangs_sel[i]:set_rect(0,0,mjwidth/55*54,mjheight/70*78);
		majiangs_sel[i].pos = Pixel(0, 0);
		majiangs_sel[i].layer = 500;
		majiangs_sel[i].display = false;
	end
	majiangs_count = index;
	
	for i=0,(majiangs_count-1) do
		majiangs[i].display = false;
		majiangs_exist[i] = 0;
		majiangs_disable[i] = 0;
	end
	local count = int((game_mjcount_w*game_mjcount_h)/majiangs_zubie);
	random_table(majiangs_double);--count);--(game_mjcount_w*game_mjcount_w));
	local mjs = {};
	local indexcnt = 0;
	for i=0,(count-1) do
		for j=0,(majiangs_zubie-1) do
			mjs[indexcnt] = random_get(i)*majiangs_zubie+j;
			indexcnt=indexcnt+1;
		end
	end
	local cnt = (game_mjcount_w*game_mjcount_h)%majiangs_zubie;
	if cnt > 0 then
		for j=0,(cnt-1) do
			mjs[indexcnt] = random_get(count)*majiangs_zubie+j;
			indexcnt=indexcnt+1;
		end
	end
	local index = 0;
	local width = fl_region.width;
	local height = fl_region.height;
	game_mj_beginX = (width - (game_mjcount_w * mjwidth)) / 2;
	game_mj_beginY = (height - (game_mjcount_h * mjheight)) / 2;
	
	game_mj_index = {};
	game_mj_count = 0;
	local cnt = 0;
	local maxtime = 1000 / (game_mjcount_w*game_mjcount_h);
	if maxtime > 30 then
		maxtime = 30;
	end
	--game_mjcount_w = config:id_i(getvali("current"), "宽数量");
	--game_mjcount_h = 
	local tbl = config:id_t(getvali("current"), "关卡表");
	local tempmj = {};
	local tempcnt = 0;
	local bExistMission = false;
	if tbl ~= "" then
		
		local mission = table_load("missions/"..tbl);
		bExistMission = true;
		for i=0,(game_mjcount_w-1) do
			tempmj[i] = {};
			for j=0,(game_mjcount_h-1) do
				tempmj[i][j] = mission:read_i(j,i);
				if tempmj[i][j] > 0 then
					tempcnt = tempcnt+1;
				end
			end
		end
		table_delete(mission);
	else
		tempcnt = mjcount_tbl;
	end
	random_table(tempcnt);--game_mjcount_w*game_mjcount_h);
	--fl_region.rect = Rect(0,0,gamewidth(),gameheight()-100);
	local mjsss = 0;
	local disableMj = 1;
	-- mjs[0] = majiangs_zubie*10;
	-- mjs[1] = majiangs_zubie*11;
	-- mjs[2] = majiangs_zubie*12;
	-- mjs[3] = majiangs_zubie*11+1;
	-- mjs[4] = majiangs_zubie*12+1;
	-- mjs[5] = majiangs_zubie*10+1;
	local mjlists = {};
	for k=0,(tempcnt-1) do
		mjlists[k] = random_get(k);
	end
	random_table(game_mjcount_w*game_mjcount_h);
	for i=0,(fl_effect_count-1) do
		fl_effect_play[i] = 0;
	end
	game_mj_exists = 0;
	for i=0,(game_mjcount_w-1) do
		game_mj[i] = {};
		for j=0,(game_mjcount_h-1) do
			game_mj[i][j] = -1;
		end
	end
	for imj=0,(tempcnt-1) do
			local indexs = random_get(imj);
			local x = (indexs%game_mjcount_w);
			local y = int(indexs/game_mjcount_w);
			local mjindex = mjs[mjlists[imj]];
			local mjsfind = false;
			while true do
				local tempmj2 = disableMj;
				for k=0,(tempcnt-1) do
					if int(mjs[k]/majiangs_zubie) == int(disableMj/majiangs_zubie) then
						disableMj = disableMj+majiangs_zubie;
						break;
					end
				end
				if tempmj2 == disableMj then
					break;
				end
			end
			--print("mjindex:"..mjsss..","..random_get(mjsss)..","..mjindex..","..disableMj);
			if bExistMission==false or tempmj[x][y]~=0 then
				if tbl ~= "" and tempmj[x][y]<0 then
					mjindex = disableMj;--mjs[tempcnt];
					disableMj=disableMj+1;
					if tempmj[x][y] < 0 then
						majiangs[mjindex]:image_mode("不可移动");
						majiangs[mjindex]:set_rect(0,0,mjwidth/55*54,mjheight/70*78);
						majiangs_disable[mjindex] = 1;
					--else
						--majiangs_disable[mjindex] = -1;
						--majiangs[mjindex]:image_mode("space");
					end
					
				else
					majiangs_disable[mjindex] = 0;
					game_mj_exists = game_mj_exists+1;
				end
				game_mj[x][y] = mjindex;
				majiangs_exist[mjindex] = 1;
				game_mj_index[game_mj_count] = mjindex;
				majiangs[mjindex].context = x*1000 + y;
				majiangs[mjindex].display = false;
				
				majiangs[mjindex].layer = x + y * game_mjcount_w;
				majiangs[mjindex].pos = Pixel(game_mj_beginX + x * mjwidth, game_mj_beginY + y * mjheight);
				majiangs_pos[mjindex] = Pixel(game_mj_beginX + x * mjwidth, game_mj_beginY + y * mjheight);
				majiangs[mjindex].color = color4(1,1,1,1);
				if bExistMission==false or (tbl ~= "" and tempmj[x][y]>0) then
					mjsss = mjsss + 1;
				else
					--mjsss = mjsss + 1;
				end
				game_mj_count = game_mj_count+1;
				
				timeout2("麻将渐显出来", cnt*maxtime, mjindex);
				cnt = cnt + 1;
			else
			game_mj[x][y] = -1;--mjindex;
			end
	end
	game_rect = Rect(game_mj_beginX, game_mj_beginY, game_mj_beginX+game_mjcount_w * mjwidth,game_mj_beginY+game_mjcount_h * mjheight);
	UpdateRegionBG();
	--game_rect.top = 0;
	播放音效("begin");
	fl_region:sort_draws();--对渲染目标进行层次排序
	game_begin = true;
	g_mousedisable = false;
	timeout("停止粒子播放", 1000);
	timeout("结束粒子播放", 1500);
	game_time_enable = true;
	刷新工具使用();
	cleartime("刷新时间使用");
	--开始时显示帮助();
	--打印gamemj();
	fl_game:setvalt("剩余值", game_mj_exists.."/"..game_mj_exists);
end
function 打印gamemj()
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			print("gamemj["..x.."]["..y.."]="..game_mj[x][y]);
		end
	end
	for i=0,(game_mj_count-1) do
		local mj = game_mj_index[i];
		local d = 1;
		if majiangs[mj].display==false then
		d=0;
		end
		print("majiang:"..mj..":display="..d..","..majiangs[mj].pos.x..","..majiangs[mj].pos.y);
	end
end
function UpdateRegionBG()
	local grect = Rect(game_mj_beginX, game_mj_beginY, game_mj_beginX+game_mjcount_w * mjwidth,game_mj_beginY+game_mjcount_h * mjheight);
	local regionbg = fl_game:search("regionbg");
	local x = fl_region.left;
	local y = fl_region.top;
	regionbg.rect = Rect(grect.left + x-2, grect.top+y-2, grect.right+x+2, grect.bottom+y+(mjheight/6));
end
--function 更新法力值()

--end
b双手指按下 = false;
双手指按下点 = nil;
function 双手指按下(pt1,pt2)
	b双手指按下 = true;
	双手指按下点 = pt1;
	local mj = 坐标下的麻将(pt2);
	if mj ~= nil then
		if 是否可消除此麻将(mouse_down, mj) then
			消除两个麻将(mouse_down, mj, true);
			mouse_down = nil;
			mouse_down_mj = nil;
			b双手指按下 = false;
		end
	end
end

function 双手指移动(pt)
	if game_rect==nil or game_begin == false then
		return;
	end
	if b双手指按下 ==false then
		return;
	end
	local off = pt - 双手指按下点;
	local left = fl_region.left - game_rect.left;
	if left < 0 then
		left = left + off.x;
		if left > 0 then
			left = 0;
		end
	end
	--fl_region.left = left + game_rect.left;
	game_mj_beginX = game_mj_beginX + off.x;
	game_mj_beginY = game_mj_beginY + off.y;
	local width = mjwidth*(game_mjcount_w) - fl_region.width;
	local height = mjheight*(game_mjcount_h) - fl_region.height;
	if game_rect.left < 0 then
		if game_mj_beginX > 0 then
			game_mj_beginX = 0;
		end
		if game_mj_beginX < -width then
			game_mj_beginX = -width;
		end
	else
		game_mj_beginX = game_rect.left;
	end
	if game_rect.top < 0 then
		if game_mj_beginY > 0 then
			game_mj_beginY = 0;
		end
		if game_mj_beginY < -height then
			game_mj_beginY = -height;
		end
	else
		game_mj_beginY = game_rect.top;
	end
	if game_mj_beginX ~= game_rect.left or game_mj_beginY ~= game_rect.top then
		for i=0,(game_mjcount_w-1) do
			for j=0,(game_mjcount_h-1) do
				local mjindex = game_mj[i][j];
				if mjindex >= 0 then
					majiangs[mjindex].pos = Pixel(game_mj_beginX + i * mjwidth, game_mj_beginY + j * mjheight);
					majiangs_pos[mjindex] = Pixel(game_mj_beginX + i * mjwidth, game_mj_beginY + j * mjheight);
				end
			end
		end
	end
	UpdateRegionBG();
	双手指按下点 = pt;
end

function 双手指弹起(pt)
	b双手指按下 = false;
end
function 停止粒子播放()
	for i=0,(fl_effect_count-1) do
		fl_effect[i].effect_stop = true;
	end
end
function 结束粒子播放()
	for i=0,(fl_effect_count-1) do
		fl_effect[i].display = false;
	end
end
function 麻将持续渐显出来(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	local index = atoi(param);
	tm = tm*5;
	majiangs[index].opacity = tm;
	if tm > 1 then
		breaktime();
		majiangs[index].opacity = 1;
	end
end

function 麻将渐显出来(idx)
	local index = atoi(idx);
	local pos1 = Pixel(majiangs_pos[index].x, majiangs_pos[index].y);
	local sindex = index % fl_effect_count;
	fl_effect[sindex].moveleft = pos1.x+mjwidth/4;
	fl_effect[sindex].movetop = pos1.y+mjheight/2;
	fl_effect[sindex].width = mjwidth;
	fl_effect[sindex].height = mjheight;
	--if fl_effect_play[sindex] == 0 then
		fl_effect_play[sindex] = 1;
		fl_effect[sindex].action = "Track";
		fl_effect[sindex].display = true;
		fl_effect[sindex].effect_loop = 1;
	--end
	majiangs[index].display = true;
	majiangs[index].opacity = 0;
	interval2("麻将持续渐显出来", 50, 200, idx);
end
lastsel_mj = nil;
function getmin(v1,v2)
	if v1>v2 then
		return v2;
	end
	return v1;
end
function getmax(v1,v2)
	if v1>v2 then
		return v1;
	end
	return v2;
end
function 按照麻将位置取得坐标(pos)
	local ret =  Pixel(int((pos.x - game_mj_beginX + mjwidth/2) / mjwidth),int((pos.y - game_mj_beginY+mjheight/2)/mjheight));
	if ret.x < 0 then
		ret.x = 0;
	elseif ret.x >= game_mjcount_w then
		ret.x = game_mjcount_w-1;
	end
	if ret.y < 0 then
		ret.y = 0;
	elseif ret.y >= game_mjcount_h then
		ret.y = game_mjcount_h-1;
	end
	return ret;
end
function 播放消除特效(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	tm = tm * 5.0;
	local index = atoi(param);
	-- local id = int(tm/0.2);
	-- local xx = int(id/2);
	-- local yy = int(id%2);
	-- if xx > 1 then
		-- xx = 1;
	-- end
	mj_effects[index]:image_rect(0,0,256,256);
	mj_effects[index].scale = 2-tm*2;--math.sin(tm*5+1)*2;
	mj_effects[index].opacity = 1-tm*0.8;--math.sin(tm*5);
	--majiangs[mj_effects[index].context].opacity = (2-tm);
	if tm > 1 then--id >= 4 then
		breaktime();
		mj_effects[index].display = false;
		majiangs[mj_effects[index].context].display = false;
	end
end
function 播放消除特效等待(tm)
	interval2("播放消除特效", 50,10000, tm);
end
function 创建消除效果(pos, mjindex, wait)
	local index = mj_effect_count;
	for i=0,(mj_effect_count-1) do
		if mj_effects[i].display == false then
			index = i;
			break;
		end
	end
	if mj_effects[index] == nil then
		mj_effects[index] = draw_create();
		mj_effects[index]:image("images/slace.png");
		mj_effects[index]:image_rect(0, 0, 256, 256);
		mj_effects[index].center = Pixel(64,64);
		mj_effects[index]:set_rect(0, 0, 128, 128);
		mj_effects[index].clickscale = 1;
		mj_effects[index].layer = 100;
		fl_region:bind_draw(mj_effects[index]);
	end
	if index == mj_effect_count then
		mj_effect_count = mj_effect_count+1;
	end
	mj_effects[index].display = true;
	mj_effects[index].opacity = 0;
	mj_effects[index].context = mjindex;
	mj_effects[index].pos = Pixel(game_mj_beginX+pos.x * mjwidth-mjwidth/4, game_mj_beginY+pos.y * mjheight);
	interval2("播放消除特效", 50, 10000, index);
	--timeout_1(wait, "播放消除特效等待", index);
end
effect_c = nil;
effect_pos = {};
effect_pos_count = 0;
effect_speed = 10;

function 按照路径播放粒子计算(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	local index = atoi(param);
	local count = xiaochu_pos_count[index];
	tm = tm*effect_speed;
	local findex = int(tm);
	local findex2 = findex+1;
	if findex2 >= count then
		effect_c.pos = xiaochu_pos[index][count-1] + Pixel(mjwidth/4,0);
		effect_c.effect_stop = true;
		breaktime();
	else
		local ltm = tm - findex;
		local dir = xiaochu_pos[index][findex2] - xiaochu_pos[index][findex];
		if ltm > 1 then
			ltm = 1;
		end
		effect_c.pos = Pixel(xiaochu_pos[index][findex].x+dir.x*ltm, xiaochu_pos[index][findex].y+dir.y*ltm);
	end
end
function 按照路径播放粒子等待(sindex)
	local index = atoi(sindex);
	effect_c.display = true;
	effect_c.pos = xiaochu_pos[index][0];
	effect_c.action = "Track";
	interval2("按照路径播放粒子计算", 50, 10000, sindex);
end
function 按照路径播放粒子(eff, speed)
	effect_c	=	eff;
	effect_c.width = mjwidth/4;
	effect_c.height = mjheight/4;
	effect_speed = speed;
	local xc = xiaochu_index%3;
	xiaochu_pos[xc] = {};
	for i=0,(effect_pos_count-1) do
		xiaochu_pos[xc][i] = effect_pos[i] + Pixel(mjwidth/1.2,mjheight/4);
	end
	xiaochu_pos_count[xc] = effect_pos_count;
	timeout2("按照路径播放粒子等待", 200, xc);
end
g_zhijiao_pos = nil;
g_zhijiao_pos2 = nil;
function 是否有阻挡X(xfrom, xto, y, exc1,exc2)
    if y<0 or y>=game_mjcount_h then
        return false;
    end
    local x1 = getmin(xfrom, xto);
    local x2 = getmax(xfrom, xto);
    for x=x1,x2 do
        if x<0 or x>=game_mjcount_w then
        elseif game_mj[x][y]>=0 then
			if game_mj[x][y]==exc1 or game_mj[x][y]==exc2 then
			elseif majiangs_disable[game_mj[x][y]]~=0 then
				return true;
			else
				return true;
			end
        end
    end
    return false;
end
function 是否有阻挡Y(yfrom, yto, x, exc1,exc2)
    if x<0 or x>=game_mjcount_w then
        return false;
    end
    local y1 = getmin(yfrom, yto);
    local y2 = getmax(yfrom, yto);
    for y=y1,y2 do
        if y<0 or y>=game_mjcount_h then
        elseif game_mj[x][y]>=0 then
			if game_mj[x][y]==exc1 or game_mj[x][y]==exc2 then
			elseif majiangs_disable[game_mj[x][y]]~=0 then
				return true;
			else
				return true;
			end
        end
    end
    return false;
end
function 添加新消除点(x,y)
	effect_pos[effect_pos_count] = Pixel(x,y);
	effect_pos_count = effect_pos_count+1;
	if effect_pos_count > 1 then
		effect_pos[effect_pos_count] = Pixel(x,y);
		effect_pos_count = effect_pos_count+1;
	end
end
function 根据ID返回Pos(index)
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			if game_mj[x][y] == index then
				return Pixel(x,y);
			end
		end
	end
	return nil;
end
function 是否可消除(index1, index2, addpoint)
	if index1 == index2 then
		return false;
	end
	if majiangs_disable[index1] ~= 0 or majiangs_disable[index2] ~= 0 then
		return false;
	end
	if int(index1/majiangs_zubie) ~= int(index2/majiangs_zubie) then--如果两个是同一个麻将
		return false;
	end
	if index1 < 0 or index2 < 0 then
		return false;
	end
	local mj1 = majiangs[index1];
	local mj2 = majiangs[index2];
	local m1x = -1;
	local m1y = -1;
	local m2x = -1;
	local m2y = -1;
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			if game_mj[x][y] == index1 then
				m1x = x;
				m1y = y;
				break;
			end
		end
	end
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			if game_mj[x][y] == index2 then
				m2x = x;
				m2y = y;
				break;
			end
		end
	end
	--print("pos:m1x="..m1x..",m2x="..m2x..",m1y="..m1y..",m2y="..m2y..","..index1..","..index2);
	if m1x<0 or m2x <0 then
		return false;
	end
	local dx = m2x - m1x;
	local dy = m2y - m1y;
	local ldx = abs(dx);
	local ldy = abs(dy);
	if addpoint then
		effect_pos_count = 0;
		添加新消除点(m1x,m1y);
	end
	if m1x==m2x then
		if 是否有阻挡Y(m1y, m2y, m1x, index1, index2)==false then
			if addpoint then
				添加新消除点(m2x,m2y);
			end
			return true;
		end
	elseif m1y==m2y then
		if 是否有阻挡X(m1x, m2x, m1y, index1, index2)==false then
			if addpoint then
				添加新消除点(m2x,m2y);
			end
			return true;
		end
	elseif 是否有阻挡X(m1x, m2x, m1y, index1, index2) ==false and 是否有阻挡Y(m1y, m2y, m2x, index1, index2) ==false then
		if addpoint then
			添加新消除点(m2x,m1y);
			添加新消除点(m2x,m2y);
		end
		return true;
	elseif 是否有阻挡X(m1x, m2x, m2y, index1, index2) ==false and 是否有阻挡Y(m1y, m2y, m1x, index1, index2) ==false then
		if addpoint then
			添加新消除点(m1x,m2y);
			添加新消除点(m2x,m2y);
		end
		return true;
	end
	for x=0,(m1x+1) do
        if 是否有阻挡X(m1x, m1x-x, m1y, index1, index2) then
            break;
        end
        if 是否有阻挡Y(m1y, m2y, m1x-x, index1, index2) == false and 是否有阻挡X(m1x-x, m2x, m2y, index1, index2) == false then
			--print("lian ok1"..(m1x-x)..","..m1y..","..m2y);
			if addpoint then
				if x>0 then
					添加新消除点(m1x-x,m1y);
					添加新消除点(m1x-x,m2y);
				else
					添加新消除点(m1x,m2y);
				end
				添加新消除点(m2x,m2y);
			end
			return true;
        end
	end
	for x=m1x,game_mjcount_w do
        if 是否有阻挡X(m1x, x, m1y, index1, index2) then
            break;
        end
        if 是否有阻挡Y(m1y, m2y, x, index1, index2) == false and 是否有阻挡X(x, m2x, m2y, index1, index2) == false then
			--print("lian ok2"..x..","..m1y..","..m2y);
			if addpoint then
				if x>m1x then
					添加新消除点(x,m1y);
					添加新消除点(x,m2y);
				else
					添加新消除点(m1x,m2y);
				end
				添加新消除点(m2x,m2y);
			end
			return true;
        end
	end
	for y=0,(m1y+1) do
        if 是否有阻挡Y(m1y, m1y-y, m1x, index1, index2) then
            break;
        end
        if 是否有阻挡X(m1x, m2x, m1y-y, index1, index2) == false and 是否有阻挡Y(m1y-y, m2y, m2x, index1, index2) == false then
			--print("lian ok3"..(m1y-y)..","..m1x..","..m2x);
			if addpoint then
				if y>0 then
					添加新消除点(m1x,m1y-y);
					添加新消除点(m2x,m1y-y);
				else
					添加新消除点(m2x,m1y);
				end
				添加新消除点(m2x,m2y);
			end
			return true;
        end
	end
	for y=m1y,(game_mjcount_h+1) do
        if 是否有阻挡Y(m1y, y, m1x, index1, index2) then
            break;
        end
        if 是否有阻挡X(m1x, m2x, y, index1, index2) == false and 是否有阻挡Y(y, m2y, m2x, index1, index2) == false then
			--print("lian ok4"..y..","..m1x..","..m2x);
			if addpoint then
				if y>m1y then
					添加新消除点(m1x,y);
					添加新消除点(m2x,y);
				else
					添加新消除点(m2x,m1y);
				end
				添加新消除点(m2x,m2y);
			end
			return true;
        end
	end
	return false;
end
function 是否可消除此麻将(mj1, mj2)
	local index1 = game_mj[mj1.x][mj1.y];
	local index2 = game_mj[mj2.x][mj2.y];
	if index1==index2 then
		return false;
	end
	if majiangs_disable[index1] ~= 0 or majiangs_disable[index2] ~= 0 then
		return false;
	end
	if int(index1/majiangs_zubie) == int(index2/majiangs_zubie) then--如果两个是同一个麻将
		if index1 >= 0 and index2 >= 0 and 是否可消除(index1, index2) then
			return true;
		end
	end
	return false;
end
置换麻将1 = nil;
置换麻将2 = nil;
置换麻将位置1 = nil;
置换麻将位置2 = nil;
function 两个麻将位置置换(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	tm = tm * 2;
	local pos = 置换麻将位置2 - 置换麻将位置1;
	local pos1 = Pixel(pos.x*tm+置换麻将位置1.x, pos.y*tm+置换麻将位置1.y);
	local pos2 = Pixel(pos.x*(1-tm)+置换麻将位置1.x, pos.y*(1-tm)+置换麻将位置1.y);
	majiangs[置换麻将1].pos = pos1;
	majiangs[置换麻将2].pos = pos2;
	pos1.y = pos1.y + 60;
	pos2.y = pos2.y + 60;
	fl_effect[0].pos = pos1;
	fl_effect[1].pos = pos2;
	if tm >= 1 then
		breaktime();
		g_mousedisable = false;
		按照麻将位置重新计算GameMJ();
	end
end
function Undo()
	if game_undo_count <= 0 then
		return;
	end
	game_undo_count = game_undo_count - 1;
	for i=0,(game_mj_count-1) do
		local mj = game_mj_index[i];
		majiangs[mj].display = false;
		majiangs_exist[mj] = 0;
	end
	--print("UNDO"..game_undo_count);
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			game_mj[x][y] = 	game_undos[game_undo_count][x][y];
			local mjindex = game_mj[x][y];
			if mjindex >= 0 then
				majiangs_exist[mjindex] = 1;
				majiangs_pos[mjindex] = Pixel(x*mjwidth+game_mj_beginX, y*mjheight+game_mj_beginY);
				majiangs[mjindex].pos = majiangs_pos[mjindex];
				majiangs[mjindex].display = true;
				majiangs[mjindex].color = color4(1,1,1,1);
				majiangs[mjindex].layer = x+y*game_mjcount_w;
			end
		end
	end
	fl_region:sort_draws();
	game_undos[game_undo_count] = nil;
end
function 消除两个麻将(mj1,mj2, 计算游戏结束)
	--game_undos = {};
	game_undos[game_undo_count] = {};
	for i=0,(game_mjcount_w-1) do
		game_undos[game_undo_count][i] = {};
		for j=0,(game_mjcount_h-1) do
			game_undos[game_undo_count][i][j] = game_mj[i][j];
		end
	end
	game_undo_count = game_undo_count + 1;
	--print("ADDUNDO"..game_undo_count);
	local index1 = game_mj[mj1.x][mj1.y];
	local index2 = game_mj[mj2.x][mj2.y];
	if 处于置换状态 then
		g_mousedisable = true;
		majiangs_sel[0].display = false;
		if lastsel_mj~= nil then
			last_index = game_mj[lastsel_mj.x][lastsel_mj.y];
			majiangs[last_index].color = color4(1,1,1,1);
			lastsel_mj = nil;
		end
		置换麻将1 = index1;
		置换麻将2 = index2;
		置换麻将位置1 = majiangs[index1].pos;
		置换麻将位置2 = majiangs[index2].pos;
		majiangs[index1].layer = 100;
		majiangs[index2].layer = 100;
		fl_region:sort_draws();
		game_mj[mj1.x][mj1.y] = index2;
		game_mj[mj2.x][mj2.y] = index1;
		设置特效位置(fl_effect[0], mj1);
		设置特效位置(fl_effect[1], mj2);
		fl_game:enable("加时按钮",true);
		播放音效("de");
		处于置换状态 = false;
		interval("两个麻将位置置换", 50, 2000);
		return true;
	end


	local ret = false;
	if index1 >= 0 and index2 >= 0 and index1 ~= index2 and int(index1/majiangs_zubie) == int(index2/majiangs_zubie) then--如果两个是同一个麻将
		if 是否可消除(index1, index2, true)==true then
			ret = true;
			local b1 = 0;
			if majiangs[index1].display then
				b1 = 1;
			end
			local b2 = 0;
			if majiangs[index2].display then
				b2 = 1;
			end
			majiangs_exist[index1] = 0;
			majiangs_exist[index2] = 0;
			创建消除效果(mj1, index1, 0);
			创建消除效果(mj2, index2, 0);
			for i1=0,(effect_pos_count-1) do
				effect_pos[i1] = Pixel(game_mj_beginX+effect_pos[i1].x * mjwidth+45, game_mj_beginY+effect_pos[i1].y * mjheight+fl_region.top+50);
			end
			按照路径播放粒子(xiaochu_eff[(xiaochu_index+1)%3], 15);
			local current = getvali("current");
			if current < 10000 then
				mj_chanchu = readi("mj_chanchu"..current);--魔晶出现最大概率-出现数量*(魔晶出现最大概率*0.5);
			end
			--出现的越多最后越难得到,combo连击越高，魔晶出现概率更高
			local gailv_l = (g_combocount-1)/2 * (钻石出现概率*0.5);
			if gailv_l > 钻石出现概率 then
				gailv_l = 钻石出现概率;
			elseif gailv_l < 0 then
				gailv_l = 0;
			end
			local gailv = (钻石出现概率 - (mj_chanchu/最大魔晶出现数量)*(钻石出现概率*0.5)) + gailv_l;--g_combocount/5
			local bMJchuxian = false;
			if math.random() < gailv then
				if mj_chanchu >= 最大魔晶出现数量 then
				else
					bMJchuxian = true;
					出现钻石(effect_pos[0]);
					mj_chanchu = mj_chanchu + 1;
					if current < 10000 then
						writei("mj_chanchu"..current, mj_chanchu);
					end
					fl_game:setvalt("钻石值", mj_chanchu.."/"..最大魔晶出现数量);
				end
			end
			g_movedisable = false;
			if 计算连击() == false then
				if bMJchuxian then
					播放音效("gomojing");
				else
					播放音效("xiaochu");
				end
			end
			local mjc = 0;
			for x=0,(game_mjcount_w-1) do
				for y=0,(game_mjcount_h-1) do
					if game_mj[x][y]>=0 and majiangs_disable[game_mj[x][y]]==0 then
						mjc = mjc+1;
					end
				end
			end
			fl_game:setvalt("剩余值", (mjc-2).."/"..game_mj_exists);
			刷新工具使用();
		end
	end
	if lastsel_mj~= nil then
		last_index = game_mj[lastsel_mj.x][lastsel_mj.y];
		majiangs[last_index].color = color4(1,1,1,1);
		lastsel_mj = nil;
	end
	if 计算游戏结束 then
		if 按照麻将位置重新计算GameMJ() == false then
			if 是否可以随机消除() == false then
				timeout("无可消除牌", 100);
			end
		end
	end

	return ret;
end

function 更新连击()
end
function 设置文本透明(name, c)
	local combo = fl_game:search(name);
	combo.textcolor = c;
	combo.opacity = c.a;
end

function 设置全部透明(a)
	local c = color4(1,1,1,a);
	设置文本透明("连击值", c);
	设置文本透明("连击背景", c);
end
g_lianji_fx = 0;
function 持续计算连击效果(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	tm = tm * 3;
	local tmv = math.sin(tm*math.pi/(g_lianji_time+1))*80;
	if tmv > 35 then
		tmv = 35+(tmv-35)/5;
		g_lianji_fx = 1;
	elseif tmv < 0 then
		g_lianji_c.display = false;
		g_lianji_c2.display = false;
		fl_game:show("连击加时", false);
		breaktime();
	end
	g_lianji_c.opacity = tmv/35;
	g_lianji_c2.opacity = tmv/35;
	g_lianji_c.movetop = 75 - tmv;
	g_lianji_c2.movetop = 71 - tmv;
end
function 增加连续积分(n)
	local newl = getvali("jifen")+n;
	setvali("jifen", newl);
	fl_game:setvalt("积分值", "x"..getvali("jifen"));
	if newl > readi("newrecord") then
		setvali("newrecord", 1);
		writei("newrecord", newl);
	end
end
function 计算连击()
	local tm = apptime() - g_combotime;
	g_combotime = apptime();
	print("LIANJIE:"..tm..","..g_lianji_time);
	if tm < g_lianji_time then
		g_combocount = g_combocount + 1;
		if g_combocount > 1 then
			fl_game:setvalt("连击值", "+ "..g_combocount);
			local si = g_combocount;
			if si > 15 then
				si = 15;
			end
			播放音效("beiwin"..(si-1));
			interval("持续计算连击效果", 50, (g_lianji_time+1) * 1000);
			g_lianji_c.display = true;
			g_lianji_c2.display = true;
			g_lianji_c.opacity = 0;
			g_lianji_c2.opacity = 0;
		else
			增加连续积分(1);
			return false;
		end
	else
		增加连续积分(1);
		g_lianji_c.display = false;
		g_lianji_c2.display = false;
		fl_game:setvalt("连击值", "0");
		g_combocount = 0;
		return false;
	end

	
	local combocountt = round(int(g_combocount/2));
	if combocountt > 2 then
		combocountt = 2;
	end
	local combocountg = round(math.pow(int(g_combocount/1.5), 1.5));
	if combocountt > 0 then--g_combocountt then
		fl_game:show("连击加时", true);
		fl_game:search("连击加时").action = "ani";
		fl_game:bind_event("onplayover", "连击加时", "隐藏控件");
		local dt = combocountt;--(combocountt-g_combocountt);--*100;
		--print("addtime:"..dt);
		game_time = game_time + dt;
		if game_time > game_time_length then
			game_time = game_time_length;
		end
		g_timeadd = g_timeadd + dt;
		g_combocountt = combocountt;
		更新时间计数(game_time);
	end
	if combocountg > 0 then --g_combocountg then
		local dt = combocountg;-- - g_combocountg;
		增加连续积分(g_combocount);
		g_combocountg = combocountg;
	end
	fl_game:setvalt("连击加时", "+"..combocountt);
	--combo ×2：时间 round(1秒*int(combo次数/2),0)
	--combo ×5：魔晶 round(1.5*int(combo次数/5),0)
	更新连击();
	return true;
end
function 无可消除牌()
	重置游戏并回到游戏();
end
function 选择一个麻将(mj)
	local last_index;
	local index = game_mj[mj.x][mj.y];
	if lastsel_mj~= nil then
		last_index = game_mj[lastsel_mj.x][lastsel_mj.y];
		majiangs[last_index].color = color4(1,1,1,1);
		if last_index ~= index then
			if 消除两个麻将(lastsel_mj, mj, true) then
				return;
			else
				--播放音效("dropno");
			end
			lastsel_mj = nil;
		else
			lastsel_mj = nil;
		end
		--return;
	end
	if index >= 0 then
		播放音效("selmj");
		lastsel_mj = mj;
		last_index = game_mj[lastsel_mj.x][lastsel_mj.y];
		majiangs[last_index].color = color4(0.5,0.5,0.5,1);
		if 处于置换状态 then
			majiangs_sel[0].pos = Pixel(game_mj_beginX + lastsel_mj.x * mjwidth, game_mj_beginY + lastsel_mj.y * mjheight);
			majiangs_sel[0].display = true;
		end
	end
end

手指按下值 = nil;
手指按下点 = nil;
function PtInPos(pt, pos1, pos2)
	if pt.x < pos1.x or pt.y < pos1.y or pt.x>pos2.x or pt.y >pos2.y then
		return false;
	end
	return true;
end
function PtInRect(rect,pt)
	if pt.x < rect.left or pt.y < rect.top or pt.x>rect.right or pt.y >rect.bottom then
		return false;
	end
	return true;
end
mouse_down_mj = nil;
mouse_down = nil;
mouse_down_mjs = {};
mouse_down_mjcount = 0;
mouse_dir_x = 0;
mouse_dir_y = 0;
mouse_mj_clamp = 0;
function 坐标下的麻将(pt)
	if game_begin then
		for i=0,(game_mjcount_w-1) do
			for j=0,(game_mjcount_h-1) do
				if game_mj[i][j] >= 0 then

					if majiangs_exist[game_mj[i][j]] > 0 then
					else
					--print("ERROR:NoDisplay"..i..","..j..",".."|"..game_mj[i][j]);
					end
					local mjrect = majiangs[game_mj[i][j]]:draw_rect();
					mjrect.bottom = mjrect.bottom - mjheight*(8/70);
					if PtInRect(mjrect, pt) then
						if majiangs_disable[game_mj[i][j]]>0 and 处于置换状态 == false then
							fl_game:show("提示不能消除", true);
							fl_game:search("提示不能消除").action = "ani";
							fl_game:bind_event("onplayover", "提示不能消除", "隐藏控件");
							return nil;
						end
						return Pixel(i,j);
					end
				end
			end
		end
	end
	return nil;
end
last_movein_mj = nil;
lastwaveplay = -1;
lastdisabled = false;
down_first = false;

function 手指按下(pos)--(pos=Pixel)pos是Pixel类型参数
	pt = fl_region:mousepoint();
	if g_mousedisable == true then
		return;
	end
	local rc = fl_region.rect;
	if point_in(rc, pt) == false then
		return;
	end
	手指按下值 = pt;
	手指按下点 = pt;
	mouse_dir_x = 0;
	mouse_dir_y = 0;
	mouse_mj_clamp = 0;
	mouse_down = nil;
	local ptin = Pixel(pt.x - fl_region.left - 20, pt.y - fl_region.top - 20);
	mouse_down_mjs = {};
	mouse_down_mjcount = 0;
	mouse_down_mj = nil;
	down_first = true;
	local mj = 坐标下的麻将(pt);
	majiangs_sel[0].display = false;
	majiangs_sel[1].display = false;

	if lastsel_mj == mj then
		lastsel_mj = nil;
		last_index = -1;
		if mj ~= nil then
			local lindex = game_mj[mj.x][mj.y];
			majiangs[lindex].color = color4(1,1,1,1);
		end
		return;
	end
	if mj ~= nil then
		mouse_down = mj;
		mouse_down_mj = game_mj[mj.x][mj.y];
		last_movein_mj = Pixel(-1,-1);
		lastwaveplay = -1;
		lastdisabled = false;
		选择一个麻将(mouse_down);
		--创建消除效果(mouse_down);
	end
end
game_mj_can_linear = false;

function 手指移动(pt)
	if 手指按下值==nil then
		return;
	end
	pt = fl_region:mousepoint();
	local off = pt - 手指按下值;
	game_mj_can_linear = false;
	local waveplay = 0;
end
function 判断线上是否有可消除X(mjpos, mjindex)
	if mjpos.x < (game_mjcount_w-1) then
		for x=(mjpos.x+1),(game_mjcount_w-1) do
			local index = game_mj[x][mjpos.y];
			if index >= 0 then
				if majiangs_disable[index] ~= 0 then
					break;
				end
				if int(index/majiangs_zubie) == int(mjindex/majiangs_zubie) then
					return true;
				end
				break;
			end
		end
	end
	if mjpos.x > 0 then
		for x=1,mjpos.x do
			local index = game_mj[mjpos.x - x][mjpos.y];
			if index >= 0 then
				if majiangs_disable[index] ~= 0 then
					break;
				end
				if int(index/majiangs_zubie) == int(mjindex/majiangs_zubie) then
					return true;
				end
				break;
			end
		end
	end
	return false;
end
function 隐藏可消除两对()
	majiangs_sel[0].display = false;
	majiangs_sel[1].display = false;
end
function 显示可消除两对(mj1, mj2)
		majiangs_sel[0].pos = Pixel(game_mj_beginX + mj1.x * mjwidth, game_mj_beginY + mj1.y * mjheight);
		majiangs_sel[1].pos = Pixel(game_mj_beginX + mj2.x * mjwidth, game_mj_beginY + mj2.y * mjheight);
		majiangs_sel[0].display = true;
		majiangs_sel[1].display = true;
end
function 判断线上是否有可消除Y(mjpos, mjindex)
	--隐藏可消除两对();
	if mjpos.y < (game_mjcount_h-1) then
		for y=(mjpos.y+1),(game_mjcount_h-1) do
			local index = game_mj[mjpos.x][y];
			if index >= 0 then
				if majiangs_disable[index] ~= 0 then
					break;
				end
				if int(index/majiangs_zubie) == int(mjindex/majiangs_zubie) then
					--显示可消除两对(mjpos, Pixel(mjpos.x,y));
					return true;
				end
				break;
			end
		end
	end
	if mjpos.y > 0 then
		for y=1,mjpos.y do
			local index = game_mj[mjpos.x][mjpos.y - y];
			if index >= 0 then
				if majiangs_disable[index] ~= 0 then
					break;
				end
				if int(index/majiangs_zubie) == int(mjindex/majiangs_zubie) then
					--显示可消除两对(mjpos, Pixel(mjpos.x,y));
					return true;
				end
				break;
			end
		end
	end
	return false;
end
function 按照麻将位置重新计算GameMJ()
	local gameover = true;
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			game_mj[x][y] = -1;
		end
	end
	for i=0,(game_mj_count-1) do
		local mjindex = game_mj_index[i];
		local mj = majiangs[mjindex];
		if majiangs[mjindex].display == true then
		local mjpos = 按照麻将位置取得坐标(majiangs[mjindex].pos);
		local mjx = mjpos.x;
		local mjy = mjpos.y;
		majiangs_pos[mjindex] = Pixel(mjx*mjwidth+game_mj_beginX, mjy*mjheight+game_mj_beginY);
		if majiangs_exist[mjindex] > 0 then
			if game_mj[mjx][mjy] >= 0 then
				--print("error:"..mjx..","..mjy..","..mjindex);
			end
			if majiangs_disable[mjindex] == 0 then
				gameover = false;
			end
			game_mj[mjx][mjy] = mjindex;
			majiangs[mjindex].layer = mjx+mjy*game_mjcount_w;
		end
		end
	end
	for x=0,(game_mjcount_w-1) do
		for y=0,(game_mjcount_h-1) do
			local mjindex = game_mj[x][y];
			if mjindex >= 0 then
				majiangs_pos[mjindex] = Pixel(x*mjwidth+game_mj_beginX, y*mjheight+game_mj_beginY);
				majiangs[mjindex].pos = majiangs_pos[mjindex];
				majiangs[mjindex].display = true;
			end
		end
	end
	fl_region:sort_draws();
	if gameover then
		g_mousedisable = true;
		cleartime("时间减少即时");
		timeout("结束游戏等待", 500);
	end
	return gameover;
end

function 结束游戏等待()
	if getvali("current") == 10 then
		writei("shuangjie", 1);
	elseif getvali("current") == 20 then
		writei("quanjie", 1);
	end
	local nextstar = readi("ok_"..(getvali("current")+1));
	if nextstar == 0 then
		writei("ok_"..(getvali("current")+1), 1);--2=1颗星
	end
	end_time = localtime();
	播放音效("win");
	cleartime("时间减少即时");
	timeout("成功过关游戏",1000);
end
fl_game_over = nil;
function 显示游戏结束框(b)
	if fl_game_over == nil then
		fl_game_over = dlg_load("t_gameover");
	end
	cleartime("时间减少即时");
	fl_game_over.pos = Pixel(0,0);
	弹出对话框("t_gameover");
	fl_game_over.domodal = true;
	fl_game_over:bind_click("重来", "重新玩游戏");
	fl_game_over:bind_click("下一关", "NextMission");
	fl_game_over.display = true;
	fl_game_over:show("完成关卡", false);
	fl_game_over:show("暂停游戏", false);
	fl_game_over:show("新纪录诞生", false);
	fl_game_over:show("计时结束", false);
	if insystem("android") then
		ad_show(1, true);
	end
end
function NextMission()
	if fl_game_over ~= nil then
		ad_show(1, false);
		dlg_delete(fl_game_over);
		fl_game_over = nil;
	end
		local oldi = getvali("current");
		local newi = oldi+1;
		if oldi >= 10000 then
			g_savecombocount = g_combocount;
			local mods = getvali("beginsj")+1;
			-- if mods >= 10100 then
				-- mods = 10000;
			-- end
			setvali("beginsj", mods);
			newi = 10000;
			--newi = int(math.random()*4.9) + mods;
			print("NEWI:"..newi);
			local newl = getvali("lianxu")+1;
			setvali("lianxu", newl);
			if newl > readi("lianxu") then
				setvali("newrecord", 1);
				writei("lianxu", newl);
			end
			--if (newi%5)==0 then
				--newi = 10000 + int(math.random()*4)*5;
			--end
			game_mjcount_w = int(math.random()*10.6+3);--(int(mods/2)%13);--config:id_i(getvali("current"), "宽数量");
			game_mjcount_h = int(math.random()*5.6+2);--config:id_i(getvali("current"), "高数量");
			majiangs_zubie = (int(mods/4)*2 % 6)+2;--config:id_i(getvali("current"), "组别");
			local num = int(game_mjcount_w*game_mjcount_h/4);
			mjcount_tbl = num*2 + int(num*math.random()/2)*2;--int(mods/5);--每5关增进一次
			mjsize_tbl = int((145-35*(game_mjcount_h/7))/5)*5 * 55/100;--config:id_i(getvali("current"), "麻将大小") * 55 / 100;
			timeout("可以继续连击",500);
		else
			local ri = readi("current");
			if ri < newi then
				writei("current", newi);
			end
			game_mjcount_w = config:id_i(getvali("current"), "宽数量");
			game_mjcount_h = config:id_i(getvali("current"), "高数量");
			majiangs_zubie = config:id_i(getvali("current"), "组别");
			mjcount_tbl = config:id_i(getvali("current"), "数量");
			mjsize_tbl = config:id_i(getvali("current"), "麻将大小") * 55 / 100;
		end
		local w = config:id_i(newi, "宽数量");
		if w == 0 then--如果没有下一关卡泽返回关卡列表
			goto("fl_point");
			return;
		end
		setvali("current", newi);--2=1颗星
		
		ResetGame();
end
function 可以继续连击()
	g_combotime = apptime();
	g_combocount	=	g_savecombocount;
end
function 重新玩游戏()
	if fl_game_over ~= nil then
		ad_show(1, false);
		dlg_delete(fl_game_over);
		fl_game_over = nil;
		game_mjcount_w = config:id_i(getvali("current"), "宽数量");
		game_mjcount_h = config:id_i(getvali("current"), "高数量");
		majiangs_zubie = config:id_i(getvali("current"), "组别");
		mjcount_tbl = config:id_i(getvali("current"), "数量");
		mjsize_tbl = config:id_i(getvali("current"), "麻将大小") * 55 / 100;
		ResetGame();
		return;
	end
end
function 游戏过关失败()
	显示游戏结束框(false);
	local 计时结束 = fl_game_over:gsearch("计时结束");
	计时结束.display = true;
	计时结束:bind_click("重来", "游戏重来");
	fl_game:show("region", false);
	fl_game:show("regionbg", false);
	播放音效("warning");
end
function 爽解过关结束()
	显示游戏结束框(false);
	local 新纪录诞生 = fl_game_over:gsearch("新纪录诞生");
	新纪录诞生.display = true;
	新纪录诞生:show("标题", false);
	新纪录诞生:show("标题全解", false);
	新纪录诞生:show("标题爽解", false);
	新纪录诞生:show("用时", false);
	新纪录诞生:show("用时值", false);
	新纪录诞生:bind_click("退出", "关闭并能返回关卡");
	新纪录诞生:setvali("完成关卡值", getvali("lianxu"));
	新纪录诞生:setvali("获得积分值", getvali("jifen"));
	setvali("lianxu", 0);
	setvali("jifen", 0);
	if getvali("newrecord") == 1 then
		setvali("newrecord", 0);
		新纪录诞生:show("标题爽解", true);
	else
		播放音效("warning");
		新纪录诞生:show("标题", true);
	end
	fl_game:show("region", false);
	fl_game:show("regionbg", false);

end
function 成功过关游戏()
	if getvali("current") >= 10000 and getvali("current") < 11000 then
		NextMission();
		print("爽解继续过关");
		return;
	end
	pause(true);
	显示游戏结束框(true);
	local 完成关卡 = fl_game_over:gsearch("完成关卡");
	完成关卡.display = true;
	local star1 = config:id_i(getvali("current"), "时间限制");
	local star2 = config:id_i(getvali("current"), "2星时间");
	local star3 = config:id_i(getvali("current"), "3星时间");
	local second = game_time_length - game_time;--timediff_s(end_time, begin_time) - g_timeadd-1;
	local rs = star1 - second;
	完成关卡:setvalt("用时值", game_time.."/"..game_time_length);
	完成关卡:enable("star1", false);
	完成关卡:enable("star2", false);
	完成关卡:enable("star3", false);
	完成关卡:bind_click("重来", "游戏重来");
	完成关卡:bind_click("下一关", "NextMission");
	local ri = readi("current");
	local newi = getvali("current");
	if ri < newi then
		writei("current", newi);
	end
	local olds = readi("ok_"..getvali("current"));
	local news = 1;
	if getvali("current") < 10000 then
		mj_chanchu = readi("mj_chanchu"..getvali("current"));
	end
	完成关卡:enable("钻石", mj_chanchu >= 最大魔晶出现数量);
	完成关卡:enable("钻石值", mj_chanchu >= 最大魔晶出现数量);
	完成关卡:setvalt("钻石值", mj_chanchu.."/"..最大魔晶出现数量);
	完成关卡:enable("用时值", second <= star3);
	完成关卡:enable("用时", second <= star3);
	--print("gametime:"..game_time..","..second..","..star3);
	if game_time >= star3 and mj_chanchu >= 最大魔晶出现数量 then
		完成关卡:enable("star1", true);
		完成关卡:enable("star2", true);
		完成关卡:enable("star3", true);
		news = 4;
	elseif game_time >= star2 then
		完成关卡:enable("star1", true);
		完成关卡:enable("star2", true);
		news = 3;
	else
		完成关卡:enable("star1", true);
		news = 2;
	end
	if news > olds then
		writei("ok_"..getvali("current"), news);
		local mjadd = 0;
		if olds==0 then
			mjadd = (news-olds-1);
		else
			mjadd = news-olds;
		end
		writei("mojing", readi("mojing")+mjadd);
	end
end

function 判断周围是否有可消除(mjfrom)
--是否可消除
end

function ClampPos(posx, posy)
	local w = (game_mjcount_w-1) * mjwidth + game_mj_beginX;
	local h = (game_mjcount_h-1) * mjheight + game_mj_beginY;
	if posx < game_mj_beginX then
		posx = game_mj_beginX;
	elseif posx > w then
		posx = w;
	end
	if posy < game_mj_beginY then
		posy = game_mj_beginY;
	elseif posy > h then
		posy = h;
	end
	return Pixel(posx,posy);--int(posx/mjwidth), int(posy/mjheight));
end
function 自动归位(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数
	local index = atoi(param);
	local pos = majiangs[index].pos;--;
	local origpos = majiangs_pos[index];
	local xok = false;
	local yok = false;
	if pos.x ~= origpos.x then
		local symx = symbol(pos.x - origpos.x);
		local speed = abs(pos.x - origpos.x) * 2;
		if speed < 80 then
			speed = 80;
		end
		if symx > 0 then
			pos.x = pos.x - symx*speed;
			if pos.x < origpos.x then
				pos.x = origpos.x;
			end
		else
			pos.x = pos.x - symx*speed;
			if pos.x > origpos.x then
				pos.x = origpos.x;
			end
		end
	else
		xok = true;
	end
	if pos.y ~= origpos.y then
		local symy = symbol(pos.y - origpos.y);
		local speed = abs(pos.y - origpos.y) / 2;
		if speed < 80 then
			speed = 80;
		end
		if symy > 0 then
			pos.y = pos.y - symy*speed;
			if pos.y < origpos.y then
				pos.y = origpos.y;
			end
		else
			pos.y = pos.y - symy*speed;
			if pos.y > origpos.y then
				pos.y = origpos.y;
			end
		end
	else
		yok = true;
	end
	if xok and yok then
		breaktime();
		if 是否可以随机消除() == false then
			timeout("无可消除牌",100);
		end
		majiangs[index].pos = origpos;
	else
		majiangs[index].pos = Pixel(pos.x, pos.y);
	end
end
function 移动完棋子自动计算()
	cleartime("自动归位");
	for mj=0,(mouse_down_mjcount-1) do
		local index = mouse_down_mjs[mj];
		majiangs[index].pos = majiangs_pos[index];
	end
	g_mousedisable = false;
	按照麻将位置重新计算GameMJ();
end
function 手指弹起(pt)
	pt = fl_region:mousepoint();
	local 重新计算 = true;
	if mouse_down_mj ~= nil then
		if mouse_dir_x ~=0 or mouse_dir_y ~=0 then
			if game_mj_can_linear then
				按照麻将位置重新计算GameMJ();
				if mouse_down_mj ~= game_mj[mouse_down.x][mouse_down.y] then
					g_movedisable = true;
				end
			end
			for mj=0,(mouse_down_mjcount-1) do
				local mjindex = mouse_down_mjs[mj];
				--if game_mj[mouse_down.x][mouse_down.y] >= 0 then
				interval2("自动归位", 50, 500, mjindex);
				重新计算 = false;
			end
			g_mousedisable = true;
			timeout("移动完棋子自动计算",100);
		else
		end
		--
	end
	if mouse_down ~= nil then--(mouse_dir_x ~=0 or mouse_dir_y ~=0) then
		local mj = 坐标下的麻将(pt);
		if mj ~= nil then
			if 是否可消除此麻将(mouse_down, mj) then
				消除两个麻将(mouse_down, mj, 重新计算);
			end
		end
	end
	mouse_down_mj = nil;
	mouse_dir_x = 0;
	mouse_dir_y = 0;
end
