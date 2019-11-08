#define	初始化	oninit
#include "fl_base.lua"
fl_point = nil;
missions = {};
missions_count = 0;
missions_number = {};
missions_star = {};
mission_page = 0;
fl_mission = nil;
fl_mission_left = 0;

function oninit()
	fl_point = dlg_load("t_point");
	fl_point:enable("background", true);
	fl_point.pos=Pixel(0,0);
	fl_point:bind_click("close", "返回主页面");
	fl_mission = fl_point:search("missions");
	fl_mission_left = fl_mission.left;
	fl_mission.width = mission_pagecount * gamewidth();
	fl_mission.left = 0;
	mission_page = readi("mission_page",0);
	print("AL"..mission_page);
	--writei("mojing", 1010000);
	--writei("quanjie", 1);
	local page_points = MISSION_PAGE_COUNT2;
	local index = 0;
	for j=1,mission_pagecount do
		index = (j-1) * page_points;
		local xbegin = (j-1) * gamewidth() + 30;
		for i=0,(page_points-1) do
			local star = readi("ok_"..(index+1), 0);
			missions[index] = draw_create();
			missions[index].context = index;
			missions[index]:image("images/bg.png");
			if (star>0) then
				missions[index]:image_mode("bg/pointbg");
			else
				missions[index]:image_mode("bg/pointno");
			end
			missions[index].center = Pixel(65,65);
			missions[index].pos = Pixel(25+(i%MISSION_WIDTH)*120 + xbegin, int(i/MISSION_WIDTH) * 120);
			missions[index]:bind_click("");
			missions_number[index] = missions[index]:add_child();
			missions_number[index]:number_mode("combo_number/", 54, 0);
			missions_number[index].pos = Pixel(0, -20);
			missions_number[index].number = (index+1);
			missions_star[index] = {};
			--writei("ok_"..(index+1), 5);
			missions[index].enabled = (star>0);
			missions_number[index].enabled = (star>0);
			if star > 4 then
				star = 4;
			end
			if star > 0 then
			for s=1,3 do
				missions_star[index][s] = missions[index]:add_child();
				missions_star[index][s]:image_mode("bg/crystal");
				missions_star[index][s].center = Pixel(38,38);
				missions_star[index][s].pos = Pixel((s-1)*38-12, 45);
				missions_star[index][s].scale = 0.5;
				missions_star[index][s].enabled = (star > s);
			end
			end
			fl_mission:bind_draw(missions[index]);
			index = index + 1;
			if index >= MAX_MISSIONS then
				break;
			end
		end
	end
	missions_count = index;
	更新页面豆();
	bind_input("down", "手指按下");
	bind_input("up", "手指弹起");
	bind_input("move", "手指移动");

	page_left = -mission_page* gamewidth();
	fl_mission.left = page_left;
end

function 进入关卡(v)
	local index = v.context;
	if missions[index].enabled then
		setvali("current", index + 1);
		local w = index*2+4;
		if w > 10 then
			w = 10;
		end
		setvali("current_w", w);
		setvali("current_h", w);
		goto("fl_game");
	end
end

手指按下值 = nil;
手指按下点 = nil;
手指是否按下 = false;
手指按下mission = nil;
function 手指按下(pt)
	if pt.y > 100 and pt.y < (gameheight()-50) then
		pt = fl_mission:mousepoint();
		cleartime("自动对齐到页面");
		手指是否按下 = true;
		手指按下值 = pt;
		手指按下点 = pt;
		for i=0,(missions_count-1) do
			local rc = missions[i]:draw_rect();
			if point_in(rc, pt) and missions[i].enabled then
				missions[i].scale = 1.2;
				手指按下mission = missions[i];
				break;
			end
		end
	end
end
function 手指移动(pt)
	if 手指按下值~= nil and abs(pt.x-手指按下值.x)<5 and abs(pt.y-手指按下值.y)<5 then
		return;
	end
	if 手指是否按下 then
		pt = fl_mission:mousepoint();
		local off = pt - 手指按下值;
		local newleft = fl_mission.left + off.x;
		if newleft > fl_mission_left then
			newleft = fl_mission_left;
		elseif newleft < (-gamewidth()*(mission_pagecount-1)) then
			newleft = (-gamewidth()*(mission_pagecount-1));
		end
		fl_mission.left = newleft;
		手指按下值 = pt;
	end
end

page_dir = 0;
page_left = 0;
function 自动对齐到页面()
	page_left = page_left + page_dir;
	page_dir = page_dir + symbol(page_dir)*50;
	local newpage_left = -mission_page* gamewidth();
	if page_dir > 0 then
		if page_left > newpage_left then
			page_left = newpage_left;
			cleartime("自动对齐到页面");
		end
	elseif page_left < newpage_left then
		page_left = newpage_left;
		cleartime("自动对齐到页面");
	end
	fl_mission.left = page_left;
end

function 手指弹起(pt)
	if 手指按下点 == nil then
		return;
	end
	pt = fl_mission:mousepoint();
	local jl = pt - 手指按下点;
	if 手指按下mission ~= nil then
		手指按下mission.scale = 1;
		手指按下mission = nil;
	end
	if 手指是否按下 then
		if (jl.x*jl.x+jl.y*jl.y)<MAX_CLICK_REGION then
			for i=0,(missions_count-1) do
				local rc = missions[i]:draw_rect();
				if point_in(rc, pt) then
					进入关卡(missions[i]);
					break;
				end
			end
		else
			local offsetX = fl_mission.left % gamewidth() - (gamewidth()/2);
			local off = 手指按下点 - pt;
			if off.x > 100 then
				--if offsetX < -200 then
				--mission_page = mission_page + 1;
				mission_page = -int(fl_mission.left / gamewidth()) + 1;
				--end
			elseif off.x < -100 then
				mission_page = -int(fl_mission.left / gamewidth());
			end

			if mission_page < 0 then
				mission_page = 0
			elseif mission_page >= mission_pagecount then
				mission_page = mission_pagecount - 1;
			end
			writei("mission_page", mission_page);
			print("sAL"..mission_page);
			local newpage_left = -mission_page * gamewidth();
			page_left = fl_mission.left;
			page_dir = symbol(newpage_left - page_left) * 100;
			interval(50, "自动对齐到页面", 10000);
			--page_left = -(mission_page-1)* gamewidth();
			更新页面豆();
		end
	end
	手指按下点 = nil;
	手指是否按下 = false;
end

function 更新页面豆()
	local title = fl_point:search("title");
	title:element_at(0).texturemode="point"..(mission_page+1);
	for i=0,(mission_pagecount-1) do
		fl_point:enable("page"..(i+1), (i==mission_page));
	end
end
