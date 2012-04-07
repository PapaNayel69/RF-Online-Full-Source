#ifndef _ErrorEventFromServer_h_
#define _ErrorEventFromServer_h_

//공격에러..
#define error_attack_hit			 0	//명중못함
#define error_attack_dst_not_live	-1	//대상이 이미 죽은상태
#define error_attack_map			-2	//대상이 다른맴상에 존재
#define error_attack_radius			-3	//사정거리를 벗어남
#define error_attack_avoid			-4	//대상이 공격을 회피함
#define error_attack_delay			-5	//공격지연시간내에 공격함
#define error_attack_invalid_dst	-6	//무효의 대상
#define error_attack_height			-7	//공격가능높이범위를 넘음
#define error_attack_userace		-8	//SF사용가능한 대상이 아님
#define error_attack_correctweapon	-9	//공격유형별 가능무기가 아님
#define error_attack_fpshortage		-10	//SF의 경우 FP가 모자람
#define error_attack_avator_corpse	-11	//자신이 시체인경우
#define error_attack_dst_corpse		-12	//대상이 시체인경우
#define	error_attack_sf_exist		-13	// 이미 적용된 SF가 있다.
#define	error_attack_few_mastery	-14	// mastery가 적어서 사용할수 없다.
#define error_attack_not_have_skill -15 // 가지고있는 스킬이 아님.
#define error_attack_not_have_force -16 // 가지고있는 포스이 아님.
#define error_attack_not_have_bullet	-17	//bullet가 없음
#define	error_attack_counter_attack	-18	// counter attack일 때는 공격을 할수 없다.
#define error_attack_force_lock		-19	// 포스잠김상태
#define	error_attack_skill_lock		-20	// 스킬 잠김
#define error_attack_unit_state		-21	//필드유닛을 착용상태라 스킬, 포스는 사용할수없음.
#define error_attack_unit_err		-22 //필드유닛오류
#define error_attack_match_bullet	-23	//무기와 구경이같은 총탄이 아님..
#define error_attack_rule			-50	//룰적인 포괄적인 에러..
#define error_attack_data			-60	//데이터적인 포괄적인 에러..
#define error_attack_dst_counterattack -100 //대상이 반격기상태임

//스킬, 포스에러..
#define error_sf_no_have_force		1	//없는포스아이템
#define error_sf_no_exist_dst		2	//없는대상
#define error_sf_shortage_mastery	3	//마스터리부족
#define error_sf_not_actable_race	4	//사용가능종족아님
#define error_sf_not_actable_dst	5	//시전대상이 올바르지않음
#define error_sf_shortage_fp		6	//FP부족
#define error_sf_act_limit			7	//효과제한으로인한 실패
#define error_sf_data_fail			8	//데이터가 안맞는거같음
#define error_sf_act_delay			9	//시전딜레이
#define error_sf_anti_cont_damage	10	//지속피해보조 안걸리는 상태임	
#define error_sf_existing_lv_better	11	//이전에걸린 효과가 더 고level이다	
#define error_sf_existing_dur_10pro	12	//이전에걸린 효과의 시간이 아직 10%이상 남았다	
#define error_sf_non_recvalbe_dst	13	//시전불가능한 대상
#define error_sf_riding_unit		14	//유닛탑승상태
#define error_sf_no_use_calss		15	//사용불가능한 클래스
	
//이동에러
#define error_move_map_move			1	//맵이동중
#define error_move_move_lock		2	//이동제한
#define error_move_run_lock			3	//뛰기제한
#define error_move_range_out		4	//위치범위밖
#define error_move_sp_lack			5	//sp가 모자름
#define error_move_not_move_state	6	//이동중이아님
#define error_move_trade_mode		7	//거래모드
#define error_move_corpse_state		8	//시체상태
#define error_move_groggy_state		9	//그로그상태

//채굴에러
#define error_mining_not_dummy		1	//맴에 없는 더미
#define error_mining_out_dummy		2	//더미의 범위를 벗어남
#define error_mining_in_motion		3	//이미 채굴중임
#define error_mining_nothing_equip	4	//장착한것이 없음
#define error_mining_not_tool		5	//채굴장비가 아님
#define error_mining_over_delay		6	//지연시간을 넘김
#define error_mining_less_delay		7	//지연시간이내
#define error_mining_nothing_battery	8	//베터리가 없다
#define error_mining_select_fail	9	//원석종류선택에러 
#define error_mining_full_inven		10	//채굴한 원석을 넣을공간이없다

//아이템 업그레이드 에러
#define error_upgrade_nothing_talik	1	//탈릭이 없다
#define error_upgrade_not_talik		2	//탈릭이 아니다.
#define error_upgrade_nothing_tool	3	//툴이 없다
#define error_upgrade_not_tool		4	//툴이 아니다.
#define error_upgrade_nothing_item	5	//대상아이템이 없다
#define error_upgrade_nothing_jewel	6	//보석이 없다
#define error_upgrade_not_jewel		7	//보석이 아니다.
#define error_upgrade_overlap_tol	8	//이미 무기에 내성탈릭이 있다
#define error_upgrade_not_able		9	//업그레이드 불가아이템
#define error_upgrade_full_upgrade	10	//풀업그레이드
#define error_upgrade_full_tailk	11	//해당 탈릭 풀
#define error_upgrade_none_upgrade	12	//업그레이드가안된 아이템(레벨을 다운시킬때)
#define miss_upgrade_random			100	//랜덤으로 업그레이드 실패
#define miss_upgrade_destroy_talik	101	//업그레이드 실패로 기존탈릭들 파괴
#define miss_upgrade_destroy_item	102	//업그레이드 실패로 대상아이템 파괴

//아이템제작 에러
#define error_make_nothing_tool		1	//툴이 없다
#define error_make_not_tool			2	//툴이 아니다.
#define error_make_full_inven		3	//인벤이 꽉참
#define error_make_nothing_material	4	//재료가없다
#define error_make_lack_material	5	//재료가 모자르다
#define error_make_mix_fail			6	//재료 조합 실패
#define miss_make_random			100	//랜덤으로 제작실패

//던젼관련 에러
#define error_dungeon_nothing_ditem	1	//던젼게이트아이템이 없다
#define error_dungeon_not_ditem		2	//던젼게이트아이템이 아니다
#define error_dungeon_full_gate		3	//최대 동시 오픈가능 던젼을 초과
#define error_dungeon_create_fail	4	//던젼생성 실패
#define error_dungeon_none_exist	5	//없는 던젼게이트
#define error_dungeon_refuse_enter	6	//입장권한 없음
#define error_dungeon_pos_setting	7	//위치세팅실패
#define error_dungeon_trans_map		8	//맵을 이동하는중
#define error_dungeon_not_here		9	//던전에 있지 않음
#define error_dungeon_not_near_gate 10	//던전과 먼위치
#define error_dungeon_dis_position	11	//던젼을 만들수없는 위치	//add(1/2)
#define error_dungeon_data_failure	100	//서버 데이터에러

//유닛관련 에러
#define error_unit_not_usable_race	1	//사용할수없는 종족
#define error_unit_check_out		2	//현재 유닛을 보관소에서 뺀상태
#define error_unit_full_unit		3	//유닛의 보유한도이상 보유함
#define error_unit_full_inven		4	//인벤이 꽉참.(키추가시)
#define error_unit_empty_slot		5	//빈 유닛슬롯임
#define error_unit_not_match_frame	6	//프레임과 매치가 안됨
#define error_unit_lack_money       7	//돈이 부족하다
#define error_unit_not_exist_part   8	//없는 파트임
#define error_unit_not_exist_frame  9	//없는 프레임임
#define error_unit_lack_non_pay		10	//미납요금이 보유달란트보다 큼
#define error_unit_lack_pay			11	//요금이 보유달란트보다 큼
#define error_unit_tuning_lv		12	//튜닝할 파트의 레벨이 모자름
#define error_unit_tuning_mastery	13  //튜닝할 파트의 마스터리가 모자름
#define error_unit_full_gauge		14	//hp가 꽉참
#define error_unit_bullet_index		15	//bullet index가 맞지않는다.
#define error_unit_full_bullet		16	//bullet의 갯수가 풀이다.
#define error_unit_no_have_back		17	//back pack이 없다..
#define error_unit_no_spare			18	//spare공간이 업다..
#define error_unit_full_max_unit	19	//존당 등장 유닛수 초과
#define error_unit_dist_parkingunit 20	//파킹유닛과 먼거리..
#define error_unit_dist_store		21	//유닛샹인과 먼거리..
#define error_unit_not_riding		22	//운전중이 아니다..
#define error_unit_not_unit_store   23	//유닛상인이 아님
#define error_unit_leave_newpos		24	//내릴위치가 너무 멀다..
#define error_unit_empty_spare		25	//빈 spare공간
#define error_unit_server_bin_error	100	//서버 binary data error

//소환관련 에러
#define error_recall_already_recalled	1	//이미 소환중임
#define error_recall_nothing_item		2	//소환아이템이 없다
#define error_recall_hp_under_30		3	//소환물의 hp가 30밑이다.
#define error_recall_fp_lack			4	//fp가 모자르다
#define error_recall_max_animus			5	//애너머스 월드당 소환 최대치 초과
#define error_recall_map_move			6	//맵이동중
#define error_recall_not_recalled		7	//소환상태가 아님
#define error_recall_target_invalid		8	//타겟이 무효임
#define error_recall_delay				9	//소환지연시간내임
#define error_recall_not_actable_race	10	//소환가능 종족이 아님
#define error_recal_few_mastery			11	//마스터리가 부족

//가드타워
#define error_tower_full_tower			1	//타워 총 가용수초과 
#define error_tower_no_have_item		2	//타워아이템을소지하지 않음
#define error_tower_no_tower_item		3	//타워아이템이 아님
#define error_tower_overlap_material	4	//재료를 중복해서 올림
#define error_tower_no_have_material	5	//재료아이템을소지하지 않음
#define error_tower_no_match_material	6	//재료아이템이 데이터와 매치되지 않음
#define error_tower_lack_material		7	//재료가 모자람
#define error_tower_no_match_amount		8	//재료수량이 데이터와 일치하지 않음
#define error_tower_loge_distance		9	//시전자와 원거리에 설치하려함
#define error_tower_not_operating_tower	10	//현재 작동중인 타워가 아니다.
#define error_tower_limit_num			11	//한 유저가 설치할수잇는 타워수를 초과
#define error_tower_data_fail			12	//데이터가 안맞는거같음
#define error_tower_no_act_skill		13	//해당 스킬을 사용할수없음
#endif
