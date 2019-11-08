mission_pagecount = 10;
MAX_CLICK_REGION = 20*20;
MISSION_WIDTH	=	7;
MISSION_PAGE_COUNT2 = MISSION_WIDTH*4;
MAX_MISSIONS = mission_pagecount*MISSION_PAGE_COUNT2;

function 返回主页面()
	播放音效("click");
	timeout_1(500, "进入下一个剧情", "fl_main");

end

function oncall(name,params, params2)
	if name=="jifen_score" then
		local n = atoi(params);
		local rjifen = readi("jifen_score");
		if n > rjifen then
			local add = n - rjifen;
			writei("mojing", readi("mojing",0)+add);
			writei("jifen_score", n);
		end
		更新钻石显示();
	elseif name == "buy_ok" then
		print("购买成功加钻石:"..params..",trans:"..params2);
		if params == "mjlink.buygem" then
			if readi("trans_"..params2, 0) == 0 then
				writei("trans_"..params2, 1);
				writei("mojing", readi("mojing",0)+10000);--购买成功，加10000钻石=6块钱
			else
				print("已经添加过了，不能重复添加数据");
			end
			更新钻石显示();
		end
	end
end
function 进入领钻石()--进入积分墙
	播放音效("click");
	ad_show(2, true);
end

sh_snd_volume = 1;--背景音乐音量
sh_snd_max = true;--背景音乐是否最大
function 开始背景音乐变小()
	if sh_snd_max then
		interval(200, "背景音乐变小", 5000);
		sh_snd_max = false;
	end
end
sh_bg_music = nil;--背景音乐
sh_bg_music_name = "";
sh_bg_audio_on = readi("audioon", 1);
function 播放背景音乐(name)
	sh_bg_music_name = name;
	if sh_bg_music ~= nil then
		stopsnd(sh_bg_music);
	end
	if readi("musicon") == 1 then
		sh_bg_music = playmp3(name);
		setsndvolume(sh_bg_music, sh_snd_volume);
	end
end
function 进入下一个剧情(v)
	goto(v);
end
function 播放音效(name)
	if sh_bg_audio_on == 1 then
		playwave(name);
	end
end
function 背景音乐变小()
	sh_snd_volume = sh_snd_volume - 0.01;
	if sh_snd_volume <= 0.85 then
		sh_snd_volume = 0.85;
		cleartime("背景音乐变小");
	end
	if sh_bg_music ~= nil then
		setsndvolume(sh_bg_music, sh_snd_volume);
	end
end
function 背景音乐变大()
	sh_snd_volume = sh_snd_volume + 0.01;
	if sh_snd_volume >= 1 then
		sh_snd_volume = 1;
		cleartime("背景音乐变大");
		sh_snd_max = true;
	end
	if sh_bg_music ~= nil then
		setsndvolume(sh_bg_music, sh_snd_volume);
	end
end

function alert(caption, msg, script)
	msgbox(caption, msg, "ok", script);
	弹出对话框("msgbox");
end
dlg_config = nil;
function 关闭系统设置()
	播放音效("quit");
	if dlg_config ~= nil then
		dlg_delete(dlg_config);
		dlg_config = nil;
	end
end
function 关闭音乐()
	if dlg_config:getvali("musicon") == 1 then
		writei("musicon", 1);
		if sh_bg_music == nil then
			播放背景音乐(sh_bg_music_name);
		end
	else
		writei("musicon", 0);
		if sh_bg_music ~= nil then
			stopsnd(sh_bg_music);
			sh_bg_music = nil;
		end
	end
end
function 关闭音效()
	sh_bg_audio_on = dlg_config:getvali("audioon");
	writei("audioon", sh_bg_audio_on);
end
function 打开系统设置()
	播放音效("window");
	dlg_config = dlg_load("config");
	dlg_config.display = true;
	dlg_config:setvali("musicon", readi("musicon", 1));
	dlg_config:setvali("audioon", readi("audioon", 1));
	dlg_config:bind_click("close", "关闭系统设置");
	dlg_config:bind_click("musicon", "关闭音乐");
	dlg_config:bind_click("audioon", "关闭音效");
	dlg_config.domodal = true;
	弹出对话框("config");
end

dlg_scale_begin = 1.0;
dlg_scale_turn = 0;
function 弹出对话框(name)
	
	dlg_scale_turn = 0;
	local dlg = dlg_search(name);
	dlg_scale_begin = 0.1;
	dlg.center = Pixel(480, 320);
	dlg.scalef = dlg_scale_begin;
	interval_1(50, "弹出对话框循环", name, 100000);
end

function 弹出对话框循环(name)
	if dlg_scale_turn == 0 then
		dlg_scale_begin = dlg_scale_begin + 0.35;
		if dlg_scale_begin > 1.2 then
			dlg_scale_begin = 1.2;
			dlg_scale_turn = 1;
		end
	else
		dlg_scale_begin = dlg_scale_begin - 0.15;
		if dlg_scale_begin < 1 then
			dlg_scale_begin = 1;
			cleartime("弹出对话框循环");
		end
	end
	local dlg = dlg_search(name);
	dlg.scalef = dlg_scale_begin;
end

function ResetScale(cname)
	local ctrl = shgame:search(cname);
	ctrl.scale = 1;
end

