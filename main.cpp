/*
	Roaring Dungeon roguelike game.
	Author: https://github.com/RedHopper
*/

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<fstream>
#include<chrono>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<random>
#include<algorithm>
#include"foo.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

glm::vec3 v3_dumm{0.0f, 0.0f, 0.0f};
constexpr float dumm_limit_max{1.0f};
constexpr float dumm_limit_min{0.0f};
float delta_time{};
int scr_w{800}, scr_h{600};
const int start_scr_w{scr_w}, start_scr_h{scr_h};
int lm_pressed{}, rm_pressed{};
bool vert_locked{};

struct render_object;

class mouse_handle
{
	public:
		GLFWwindow* window{};
		bool lm_pressed{}, rm_pressed{};
		bool lm_previous{}, rm_previous{};
		double xpos{}, ypos{};
		float n_xpos{}, n_ypos{};
		float nl_xpos{}, nl_ypos{};
		mouse_handle(GLFWwindow* window);
		void update_state();
		glm::vec2 get_coords();
		std::string get_coords_str();
		
};

glm::vec2 mouse_handle::get_coords()
{
	glm::vec2 coords{(n_xpos+1.0f)*scr_w/2.0f, (n_ypos+1.0f)*scr_h/2.0f};
	return coords;
}

std::string mouse_handle::get_coords_str()
{
	glm::vec2 coords{get_coords()};
	return std::to_string(coords[0]) + " " + std::to_string(coords[1]);
}

mouse_handle::mouse_handle(GLFWwindow* curr_window)
{
	window = curr_window;
}

void mouse_handle::update_state()
{
	if (!window)
	{
		std::cerr << "[*] mouse_handle: Warning: window object is not set\n";
		return;
	}
	glfwGetCursorPos(window, &xpos, &ypos);
	lm_previous = lm_pressed;
	rm_previous = rm_pressed;
	lm_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	rm_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	int hw{scr_w/2}, hh{scr_h/2};
	nl_xpos = n_xpos;
	nl_ypos = n_ypos;
	n_xpos = (xpos-hw)/hw;
	n_ypos = (ypos-hh)/-hh;
}

void resize_handle(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	scr_w = width;
	scr_h = height;
}

glm::vec3 get_grid_pos(short grid_pos)
{
	if (grid_pos > 0 && grid_pos < 4)
	{
		return glm::vec3(267.0f + 133.0f * (grid_pos-1), 167 + 133*2.0f, 0.0f);
	} else if (grid_pos > 3 && grid_pos < 7)
	{
		return glm::vec3(267.0f + 133.0f * (grid_pos-4), 167 + 133, 0.0f);
	} else if (grid_pos > 6 && grid_pos < 10)
	{
		return glm::vec3(267.0f + 133.0f * (grid_pos-7), 167.0f, 0.0f);
	}
	std::cerr << "[*] Warning: bad grid pos: " << grid_pos << "\n";
	return glm::vec3(134.0f, 300.0f, 0.0f);

}

void simple_write(std::string path, std::string to_write)
{
	std::fstream file;
	file.open(path.c_str(), std::ios::out);
	if (!file.is_open())
	{
		std::cerr << "[*] Error opening file for write with path: " << path << "\n";
		return;
	}
	file << to_write;
	file.close();
	return;

}

std::string simple_read(std::string path)
{
	std::fstream file;
	file.open(path.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "[*] Error reading file with path: " << path << "\n";
		return "Error reading file";
	}
	std::string result;
	
	std::string cache;
	while (getline(file, cache))
	{
		result += cache + "\n";
	}
	file.close();
	result.pop_back();
	return result;
}


class flex_shader
{
	public:
		unsigned int gl_id{};
		std::string name{"flex_shader_name_not_set"};
		flex_shader(std::string, std::string, std::string);
		void fset_uniform(std::string, float); 
		void fset_vec3(std::string, glm::vec3);
		void fset_mat4(std::string, glm::mat4);
		void fset_vec4(std::string u_name, glm::vec4 vec)
		{
			int uniform_pos{glGetUniformLocation(gl_id, u_name.c_str())};
			if (uniform_pos == -1)
			{
				std::cerr << "[*] flex_shader: Error looking for uniform with name: " << u_name << "; in shader named: " << name <<"\n";
				return;
			}
			glUniform4fv(uniform_pos, 1,  glm::value_ptr(vec));
		}
		void fset_int(std::string, int);
		unsigned int main_texture{};
};




class npc_handle
{
	public:
		std::string name;		
		int grid_pos{};
		float scale{1.0f};
		glm::vec3 vec3_scale{10.0f, 10.0f, 1.0f};
		int square_VAO{};
		flex_shader* p_shader{};
		glm::vec3 color{0.0f, 1.0f, 0.0f};
		bool pos_on_grid(const glm::vec3, const float);
		npc_handle(int, unsigned int, bool, const float, void*, int*, std::vector<render_object*>*, std::vector<flex_shader*>*, const std::vector<glm::vec3>*, std::string);
		void draw(const glm::mat4&);
		glm::vec3 gen_rand_pos(bool);
		npc_handle* check_for_collision(std::string, glm::vec3);
		bool check_for_collision_by_type(const glm::vec3&, std::string);
		npc_handle* get_player_handle();
		player_params* get_player_params();
		npc_handle* get_handle_by_name(std::string);
		std::vector<npc_handle*>* get_handles_by_type(std::string);
		void type_behaviour();
		const char* get_interactions(glm::vec3, std::string, int*);
		const char* get_grid_interactions(const glm::vec3&);
		const glm::vec3* get_nbor_poses(const glm::vec3&);
		void inc_order();
		void reset(bool);
		void end_move();
		bool pos_free(glm::vec3, bool, bool, std::string*, float);
		glm::vec3 curr_pos{};
		GLFWwindow* window{};
		glm::vec3 last_pos{};
		char moving_dir{'r'};
		char last_moving_dir{moving_dir};
		glm::vec3 target_pos{};
		bool is_moving{};
		char last_key_dir{};
		int moving_order{};
		int field_size{3};
		float moving_step{};
		void* params{};
		glm::vec3 field_ur_limit{}; //field up-right limit
		glm::vec3 field_dl_limit{}; //field down-left limit
		std::vector<flex_shader*>* all_shaders{};
		float z_pos{0.0f};
		float z_pos_add{0.3f};
		float z_pos_switch{};
		bool is_dead{};
		std::string npc_type{""};
		std::vector<npc_handle*>* all_handles{};
		int* max_morder{};
		int* curr_morder{};
		enum npc_handle_enums
		{
			rand_pos = -1,
			reset_score = 1,
			ignore_grid = 0,
			shader_flat = 1,
			shader_texture = 2,
			shader_fade = 3
		};
		int gl_texture{};
		glm::vec3 local_texture_pos{};
		bool boosted{};
		bool boosted_before{};
		struct lbutton_info lb_info{};
		bool is_static{};
		const std::vector<glm::vec3>* map_grid{};
		npc_handle* npc_dump{};
		bool lock_tex_rot{};
		bool basic_color_transparent{};
		std::vector<render_object*>* p_rinfo{};
		render_object* rinfo_fade{};
		void get_and_reverse_interactions(const glm::vec3&, std::vector<char>&, std::string, const char**, int*);
		void get_free_poses(const glm::vec3&, std::string, std::vector<glm::vec3>*);
		bool facing_pos_taken(std::string);
		bool pos_taken(const glm::vec3&, std::string, const float);
		void set_position(const glm::vec3&);
		void set_grid_position(int);
		npc_handle* get_handle_by_order(int);
		glm::vec3 fade_local_pos{0.0f, 48.0f, 0.0f};
		glm::vec3 start_fade_local_pos{fade_local_pos};
		int shader_level{shader_flat};
		bool moving_no_order{};
		void turn_till_on_grid(bool clock_wise = true, bool ignore_player = true)
		{
			if ((npc_type == "player" && ignore_player) || !map_grid->size()) return;
			for (int i{}; i < 4; ++i)
			{
				glm::vec3 new_facing_pos{curr_pos + td::dir_to_vec(moving_dir, moving_step)};
				if (!pos_taken(new_facing_pos, "slime, grid", 1.0f)) break;
				moving_dir = td::turn_dir(moving_dir, clock_wise);
			}
			//if (!pos_on_grid(curr_pos + td::dir_to_vec(moving_dir, moving_step), 0.1f)) std::cerr << "[!] nh::turn_till_on_grid - for some reason moving_dir for " << name << " is still not on grid\n"; //Debug
		}
		void set_new_target_pos(const glm::vec3& new_pos, float z_pos_mod = 0.0f)
		{
			if (z_pos_mod) z_pos_switch = z_pos_mod;
			target_pos = new_pos;
			moving_vec = target_pos - curr_pos;
			//std::cout << "Set new target_pos for " << name << "; moving_vec is: " << vec_to_str(moving_vec, 2) << "\n"; //Debug
			moving_dir = td::mvec_to_dir(moving_vec);
			new_pos_time = glfwGetTime();
		}
		npc_handle* get_handle_by_pos(const glm::vec3& pos, float error = 1.0f)
		{
			for (int i{}; i < all_handles->size(); ++i)
			{
				if (vec3_equal(all_handles->at(i)->curr_pos, pos, error)) return all_handles->at(i);
			}
			return 0;
		}
		void get_handles_by_pos(const glm::vec3& pos, std::vector<npc_handle*>* vec)
		{
			vec->clear();
			for (int i{}; i < all_handles->size(); ++i)
			{
				if (vec3_equal(all_handles->at(i)->curr_pos, pos, 0.1f)) vec->push_back(all_handles->at(i));
			}
		}
		npc_handle* get_handle_by_morder(int morder)
		{
			for (int i{}; i < all_handles->size(); ++i)
			{
				if (all_handles->at(i)->moving_order == morder) return all_handles->at(i);
			}
			return 0;
		}
		glm::vec3 moving_vec{};
		bool reset_this_turn{};
		void starting_routine()
		{
			if (npc_type != "player")
			{
				std::cerr << "[!] Warning: nh::starting_routine() - only player handle can ask for a starting routine!\n";
				return;
			}
			std::vector<glm::vec3> neigh_poses;
			td::get_neigh_poses(curr_pos, moving_step, &neigh_poses);
			int count_free_poses{};
			
			for (int i{}; i < neigh_poses.size(); ++i)
			{
				if (!pos_taken(neigh_poses[i], "slime, grid", 0.1f)) ++count_free_poses;
			}
			if (!count_free_poses) 
			{
				gen_rand_pos(true);
			}
		}
		float new_pos_time{};
		float death_time{};
		bool debug_mode{};
	private:
		int last_grid_pos{};
		bool is_npc{};
		
};

void npc_handle::get_and_reverse_interactions( const glm::vec3& pos, std::vector<char>& res_vec, std::string key_reasons, const char** inter_out = 0, int* inter_size_out = 0)
{
	int inter_size{};
	const char* interactions{get_interactions(pos, key_reasons, &inter_size)};
	res_vec.clear();
	res_vec.push_back('d');
	res_vec.push_back('u');
	res_vec.push_back('l');
	res_vec.push_back('r');
	for (int i{}; i < inter_size; ++i)
	{
		auto index{std::find(res_vec.begin(), res_vec.end(), interactions[i])};
		if (index != res_vec.end()) res_vec.erase(index);
	}
	if (inter_size_out) *inter_size_out = inter_size;
	if (inter_out) *inter_out = interactions;
	else delete[] interactions;
}

npc_handle* npc_handle::get_handle_by_order(int order)
{
	for (int i{}; i < all_handles->size(); ++i)
	{
		if (all_handles->at(i)->moving_order == order) return all_handles->at(i);
	}
	return 0;
}

void npc_handle::get_free_poses(const glm::vec3& pos, std::string key_reasons, std::vector<glm::vec3>* free_poses)
{
	std::vector<char> inter;
	get_and_reverse_interactions(pos, inter, key_reasons);
	for (int i{}; i < inter.size(); ++i)
	{
		free_poses->push_back(pos + td::dir_to_vec(inter[i], moving_step));
	}
}

const glm::vec3* npc_handle::get_nbor_poses(const glm::vec3& pos)
{
	if (!map_grid)
	{
		std::cerr << "[*]Error: npc_handle::get_nbor_poses() - map_grid is zero\n";
		return 0;
	}
	glm::vec3* cross_pos{new glm::vec3[4]{pos, pos, pos, pos}};
	cross_pos[0][0] += moving_step; 
	cross_pos[1][0] -= moving_step; 
	cross_pos[2][1] += moving_step;
	cross_pos[3][1] -= moving_step;
	return cross_pos;
}

void npc_handle::end_move() //end of move
{
	if (!all_handles || !curr_morder)
	{
		std::cerr << "[*]Error: npc_handle::end_move() - essential pointers are not set\n";
		return;
	}
	if (npc_type == "player"){
		for (int i{}; i < all_handles->size(); ++i)
		{
			npc_handle& curr_handle{*all_handles->at(i)};
			if (curr_handle.is_dead || !vec3_equal(curr_handle.curr_pos, curr_pos, 0.1f)) continue;
			if (curr_handle.npc_type == "slime" && ((td::opposite_dir(curr_handle.moving_dir) == moving_dir) || boosted_before))
			{
				//std::cout << "Slime with name: " << curr_handle.name << " was killed by player\n"; //Debug
				player_params* p_params{reinterpret_cast<player_params*>(params)};
				p_params->score += p_params->scr_death_kill;

				curr_handle.is_dead = true;
				continue;
			} else if (curr_handle.npc_type == "slime"){
				is_dead = true;
				death_time = glfwGetTime();
				*curr_morder = 0;
				continue;
			}
			else if (curr_handle.npc_type == "bat")
			{
				curr_handle.is_dead = true;
				player_params& p_params{*reinterpret_cast<player_params*>(params)};
				p_params.score += p_params.scr_bat_kill;
				//std::cout << "Bat " << curr_handle.name << " was killed by player\n"; //Debug
				continue;
			}
			else if (curr_handle.npc_type == "chest")
			{
				curr_handle.is_dead = true;
				player_params& p_params{*reinterpret_cast<player_params*>(params)};
				p_params.score += p_params.scr_chest_pick;
				//std::cout << "Chest was destroyed by player\n"; //Debug
				continue;
			}
			else if (curr_handle.npc_type == "portal")
			{
				reset(false);
				reset_this_turn = true;
			}
		}
		return;
	} else if (npc_type == "slime")
	{
		const glm::vec3 facing_pos{curr_pos + td::dir_to_vec(moving_dir, moving_step)};
		for (int i{}; i < all_handles->size(); ++i)
		{
			npc_handle& curr_handle{*all_handles->at(i)};
			if (curr_handle.is_dead || !vec3_equal(curr_handle.curr_pos, curr_pos, 0.1f) || &curr_handle == this) continue;
			if (curr_handle.npc_type  == "player")
			{
				curr_handle.is_dead = true;
				curr_handle.death_time = glfwGetTime();
				//std::cout << "Player was killed by npc with name: " << name << "\n"; //Debug
			} else if (curr_handle.npc_type == "chest")
			{
				curr_handle.is_dead = true;
				//std::cout << "Chest " << curr_handle.name << "  was destroyed by slime: " << name << "\n"; //Debug
				return;
			} else if (curr_handle.npc_type == "bat")
			{
				curr_handle.is_dead = true;
				//std::cout << "Bat was killed by slime: " << name << "\n"; //Debug
			} else if (curr_handle.npc_type == "slime")
			{
				curr_handle.is_dead = true;
				//std::cout << "Slime " << curr_handle.name << " was destroyed by " << name << "\n"; //Debug
			}

		}
		std::string obstacles{"slime, grid, bat"};
		if (pos_taken(facing_pos, obstacles, 3.0f) && !boosted ){
			bool clock_wise{rand()%2};
			turn_till_on_grid(clock_wise);
		}
		return;
	} else if (npc_type == "portal")
	{
		return;
	} else if (npc_type == "boost")
	{
		return;
	} else if (npc_type == "bat")
	{
		for (int i{}; i < all_handles->size(); ++i){
			npc_handle& curr_handle{*all_handles->at(i)};
			if (curr_handle.is_dead) continue;
			if (curr_handle.npc_type == "player" && vec3_equal(curr_pos, curr_handle.curr_pos))
			{
				curr_handle.is_dead = true;
				curr_handle.death_time = glfwGetTime();
				//std::cout << "Player was killed by bat with name: " << name << "\n"; //Debug
			} else if (curr_handle.npc_type == "slime" && boosted_before && vec3_equal(curr_pos, curr_handle.curr_pos, 1.0f))
			{
				curr_handle.is_dead = true;
				//std::cout << "Slime was killed by bat with name: " << name << "\n"; //Debug
			}
		}
		return;
	}
	std::cerr << "[!]Warning: npc_handle::end_move() - end of move for type " << npc_type << " is not set.\n";
	return;
}

void npc_handle::reset(bool reset_score = false)
{
	static int resets_amount{};
	//std::cout << "Reset number: " << ++resets_amount << "\n"; //Debug
	if (!all_handles || !max_morder || !curr_morder)
	{
		std::cerr << "[*]Error: npc_handle::reset() - some or all of crucial pointers are zero\n";
		return;
	}
	for (int i{}; i < all_handles->size(); ++i) all_handles->at(i)->is_dead = true;
	for (int i{}; i < all_handles -> size(); ++i)
	{
		npc_handle& curr_handle{*all_handles->at(i)};
		curr_handle.gen_rand_pos(true);
		curr_handle.moving_dir = td::rand_dir(rand());
		curr_handle.target_pos = curr_handle.curr_pos;
		curr_handle.last_pos = curr_handle.curr_pos;
		if (curr_handle.name == "player_npc")
		{
			curr_handle.lb_info.direction = 0;
			if (reset_score)
			{
				player_params* p_params{reinterpret_cast<player_params*>(curr_handle.params)};
				p_params->score = 0;
			}
		}
		if (curr_handle.npc_type == "portal") curr_handle.is_dead = true; 
		else curr_handle.is_dead = false;
	}
	for (int i{}; i < all_handles->size(); ++i)
	{
		(*all_handles)[i]->turn_till_on_grid();
	}
	npc_handle* npc_player{get_handle_by_name("player_npc")};
	if (npc_player){
		std::vector<glm::vec3> free_poses;
		get_free_poses(npc_player->curr_pos, "slime, grid", &free_poses);
		if (!free_poses.size())
		{
			int inter_size{};
			const char* interactions{get_interactions(npc_player->curr_pos, "slime, grid", &inter_size)};
			for (int i{}; i < inter_size; ++i)
			{
				npc_handle* curr_npc{get_handle_by_pos(npc_player->curr_pos + td::dir_to_vec(interactions[i], moving_step))};
				if (!curr_npc) continue;
				else if (curr_npc->npc_type == "slime")
				{
					curr_npc->moving_dir = td::mvec_to_dir(curr_npc->curr_pos - npc_player->curr_pos);
					std::cout << "[D] While reseting changer moving dir of " << curr_npc->name << " to the oppostie of player\n";
					break;
				}
			}
		}
	}
	*curr_morder = 0;
}

void npc_handle::inc_order() //function increases order and if order is more than maximum one, it decreases it
{
	if (!curr_morder || !max_morder)
	{
		std::cerr << "[*]Error: npc_handle::inc_order() - curr_morder or/and max_morder pointers are not set\n";
		return;
	}
	//std::cout << "My name: " << name << "; My moving_order: " << moving_order << "; and curent_morder: " << *curr_morder << "; is_dead: " << is_dead << "\n";
	++*curr_morder;
	if (*curr_morder > *max_morder) *curr_morder = 0;
}

std::vector<npc_handle*>* npc_handle::get_handles_by_type(std::string type)
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: npc_handle::get_handles_by_type() - all_handles pointer is zero\n";
		return nullptr;
	}
	std::vector<npc_handle*>* result{new std::vector<npc_handle*>};
	for (int i{}; i < all_handles->size(); ++i)
	{
		npc_handle* c_mem{all_handles->at(i)};
		if (c_mem == this) continue;
		if (c_mem->npc_type == type)
		{
			result->push_back(c_mem);
		}
	}
	return result;
}

void npc_handle::set_position(const glm::vec3& new_pos) //notes: this func assumes only one object does occupy single grid
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: all_handles is unset\n";
		return;
	}
	for (int i{}; i < all_handles->size(); ++i)
	{
		if (vec3_equal(all_handles->at(i)->curr_pos, new_pos, 0.1f))
		{
			all_handles->at(i)->curr_pos = curr_pos;
			all_handles->at(i)->target_pos = curr_pos;
			all_handles->at(i)->last_pos = curr_pos;
			break;
		}
	}
	curr_pos = new_pos;
	target_pos = new_pos;
	last_pos = new_pos;  
}

void npc_handle::set_grid_position(int new_grid_pos) //notes: this func assumes only one object does occupy single grid
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: all_handles is unset\n";
		return;
	}
	const glm::vec3 new_pos{get_grid_pos(new_grid_pos)};
	for (int i{}; i < all_handles->size(); ++i)
	{
		if (vec3_equal(all_handles->at(i)->curr_pos, new_pos, 0.1f))
		{
			all_handles->at(i)->curr_pos = curr_pos;
			all_handles->at(i)->target_pos = curr_pos;
			all_handles->at(i)->last_pos = curr_pos;
			break;
		}
	}
	curr_pos = new_pos;
	target_pos = new_pos;
	last_pos = new_pos;
}

bool npc_handle::pos_free(glm::vec3 pos, bool check_grid = true, bool ignore_dead = true, std::string* out_reason = 0, float allowed_error = 1.0f)
{
	if (!all_handles)
	{
		std::cerr << "[*]Error: npc_handle::pos_free() - all_handles pointer is not set\n";
		return false;
	}
	for (int i{}; i < all_handles->size(); ++i)
	{
		npc_handle& curr_handle{*all_handles->at(i)};
		if (vec3_equal(curr_handle.curr_pos, pos, allowed_error))
		{
			if (curr_handle.is_dead && ignore_dead) continue;
			if (out_reason) *out_reason = curr_handle.npc_type;
			return false;
		}
	}
	if (check_grid){
		if (pos_on_grid(pos, allowed_error)) return true;
		if (out_reason)	*out_reason = "grid";
		return false;
	}
	return true;
}

const char* npc_handle::get_interactions(glm::vec3 pos, std::string key_reasons, int* out_interactions_amount = 0)
{
	std::vector<std::string> vec_reasons;
	split_str(key_reasons, &vec_reasons);
	if (!map_grid)
	{
		std::cerr << "[*]Error: npc_handle::get_interactions - map_grid pointer is not set\n";
		return 0;
	}
	glm::vec3 neigh_poses[4]{};
	glm::vec3 t_pos{pos};
	t_pos[0] += moving_step;
	neigh_poses[0] = t_pos;
	t_pos = pos;
	t_pos[0] -= moving_step;
	neigh_poses[1] = t_pos;
	t_pos = pos;
	t_pos[1] += moving_step;
	neigh_poses[2] = t_pos;
	t_pos = pos;
	t_pos[1] -= moving_step;
	neigh_poses[3] = t_pos;
	char* interactions{new char[5]{}};
	int inter_size{};
	for (int i{}; i < 4; ++i)
	{
		std::string reason;
		if (!pos_free(neigh_poses[i], true, true, &reason))
		{
			auto found{std::find(vec_reasons.begin(), vec_reasons.end(), reason)};
			if (found != vec_reasons.end())
			{
				char dir{td::mvec_to_dir(neigh_poses[i]-pos)};
				interactions[inter_size] = dir;
				++inter_size;
			}
		}
	}
	if (out_interactions_amount) *out_interactions_amount = inter_size;
	return interactions;
}

const char* npc_handle::get_grid_interactions(const glm::vec3& pos)
{
	if (!map_grid)
	{
		std::cerr << "[*]Error: npc_handle::get_grid_interactions() - map_grid is zero\n";
		return 0;
	}
	char* interactions{new char[5]{}};
	int inter_size{};
	const glm::vec3* nbor_poses{get_nbor_poses(pos)};
	for (int i{}; i < 4; ++i)
	{
		for (int a{}; a < map_grid->size(); ++a)
		{
				if (vec3_equal(nbor_poses[i], map_grid->at(a)))
				{
					interactions[inter_size] = td::mvec_to_dir(map_grid->at(a) - pos);
					++inter_size;
					break;
				}
		}
	}
	delete[] nbor_poses;
	return interactions;
}


bool npc_handle::pos_taken(const glm::vec3& pos, std::string allowed_reasons, const float error = 0.00000001f)
{
	std::vector<std::string> vec_reasons;
	split_str(allowed_reasons, &vec_reasons);
	std::string reason;
	if (!pos_free(pos, true, true, &reason, error))
	{
		auto found{std::find(vec_reasons.begin(), vec_reasons.end(), reason)};
		if (found != vec_reasons.end()) return true;
	}
	return false;
}

bool npc_handle::facing_pos_taken(std::string allowed_reasons)
{
	std::vector<std::string> vec_reasons;
	split_str(allowed_reasons, &vec_reasons);
	std::string reason;
	if (!pos_free(curr_pos + td::dir_to_vec(moving_dir, moving_step), true, true, &reason))
	{
		auto found{std::find(vec_reasons.begin(), vec_reasons.end(), reason)};
		if (found != vec_reasons.end()) return true;
	}
	return false;
}

void npc_handle::type_behaviour()
{
	if (npc_type == "slime")
	{
		std::vector<char> all_pos{};
		const char* interactions;
		int inter_amount{};
		std::string obstacles{"grid, slime"};
		get_and_reverse_interactions(curr_pos, all_pos, obstacles, &interactions, &inter_amount);
		//std::cout << "Interactions for slime with obstacles " + obstacles << ": " << interactions << "\n";
		glm::vec3 facing_pos{curr_pos + td::dir_to_vec(moving_dir, moving_step)};
		bool can_move{!charin(moving_dir, interactions)};
		if (!all_pos.size())
		{
			//std::cout << "type_behaviour(npc_type == \"slime\"): All poses around me are taken by: " + obstacles + ", moving to the one with npc on it!\n"; //Debug
			get_and_reverse_interactions(curr_pos, all_pos, "grid", &interactions, &inter_amount);
			//if (!all_pos.size()) std::cout << "[I] Warning: all_pos size is zero even after checking interactions only with grid; that should not be possible\n"; //Debug
		}
		if (can_move && pos_taken(facing_pos, "bat, chest"))
		{
			std::vector<npc_handle*> npcs;
			get_handles_by_pos(facing_pos, &npcs);
			npc_handle* npc_bat{get_handle_by_name("npc_bat")};
			for (int i{}; i < npcs.size(); ++i)
			{
				npcs[i]->set_new_target_pos(curr_pos, -0.2f);
			}
		}
		//std::cout << "All possible moves: " << chvec_to_str(all_pos) << "\n";
		delete[] interactions;
		std::vector<glm::vec3> result{};
		for (int i{}; i < all_pos.size(); ++i)
		{
			result.push_back(curr_pos + td::dir_to_vec(all_pos[i], moving_step));
		}
		if (can_move)
		{
			target_pos = curr_pos + td::dir_to_vec(moving_dir, moving_step);
		}
		else if (result.size() == 1)
		{
			target_pos = result.at(0);
		}
		else if (result.size() > 1) target_pos = result.at(rand()%result.size());
		else if (!result.size())
		{
			//std::cout << "[*] Warning: npc_handle::type_behaviour() - result is zero! This shouldn't be possible!\n"; //Debug
			target_pos = curr_pos;
		}
		if (pos_taken(target_pos, "bat, chest", 0.1f))
		{
			std::vector<npc_handle*> npcs;
			get_handles_by_pos(target_pos, &npcs);
			for (int i{}; i < npcs.size(); ++i)
			npcs[i]->set_new_target_pos(curr_pos, -0.2f);
		}
		return;
		
	} else if (npc_type == "chest")
	{
		target_pos = curr_pos;
		return;
	} else if (npc_type == "boost")
	{
		target_pos = curr_pos;
		return;
	} else if (npc_type == "portal")
	{
		if (is_dead){
			bool all_dead{true};
			for (int i{}; i < all_handles->size(); ++i)
			{
				npc_handle& curr_handle{*all_handles->at(i)};
				if ((curr_handle.npc_type == "bat" || curr_handle.npc_type == "slime") && !curr_handle.is_dead )
				{
					all_dead = false;
					break;
				}
			}
			if (all_dead)
			{
				is_dead = false;
				target_pos = gen_rand_pos(true);
			}
		}
		return;
	} else if (npc_type == "bat")
	{
		std::vector<glm::vec3> possible_moves;
		glm::vec3 t_pos{curr_pos};
		t_pos[0] += moving_step;
		t_pos[1] += moving_step;
		possible_moves.push_back(t_pos);
		t_pos = curr_pos;
		t_pos[0] -= moving_step;
		t_pos[1] -= moving_step;
		possible_moves.push_back(t_pos);
		t_pos = curr_pos;
		t_pos[0] += moving_step;
		t_pos[1] -= moving_step;
		possible_moves.push_back(t_pos);
		t_pos = curr_pos;
		t_pos[0] -= moving_step;
		t_pos[1] += moving_step;
		possible_moves.push_back(t_pos);
		int player_index{-1};
		for (int i{static_cast<int>(possible_moves.size())-1}; i >= 0; --i)
		{
			if (pos_taken(possible_moves[i], "slime, grid"))
				possible_moves.erase(possible_moves.begin() + i);
		}
		for (int i{}; i < possible_moves.size(); ++i)
		{
			if (pos_taken(possible_moves[i], "player"))
			{
				player_index = i;
				break;
			}
		}
		if (player_index != -1 && possible_moves.size() > 1){
			possible_moves.erase(possible_moves.begin() + player_index);
			//std::cout << "[D] I have poses without a player, so I will choose from them!\n"; //Debug
		}
		//std::cout << "Possible moves for bat: " << possible_moves.size() << "\n";
		if (possible_moves.size() > 0)
		{
			if (possible_moves.size() == 1) target_pos = possible_moves.at(0);
			else target_pos = possible_moves[(rand()%possible_moves.size())];
		} else if (!possible_moves.size())
		{
			//std::cerr << "[!]Warning: npc_handle::type_behaviour - Bat with name " << name << " can't move anywhere! It is dead now\n"; //Debug
			is_dead = true;
		}
		return;

	} 
	std::cerr << "[*] Error: npc_handle::behaviour - Can't decide where to move npc named: " << name << "\n";
	return;
}


npc_handle* npc_handle::get_handle_by_name(std::string obj_name)
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: npc_handle::get_handle_by_name() - all_handles pointer is zero\n";
		return nullptr;
	}
	for (int i{}; i < all_handles->size(); ++i)
	{
		if (all_handles->at(i)->name == obj_name) return all_handles->at(i);
	}
	std::cerr << "[*] Warning: npc_handle::get_handle_by_name() - npc with name " << obj_name << " doesn't exist; who tried to find it: " << name << "\n";
	return nullptr;
}

npc_handle* npc_handle::check_for_collision(std::string obj_name, glm::vec3 coll_with_pos = glm::vec3(-100.0f))
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: npc_handle::check_for_collision - all_handles variable is not set\n";
		return nullptr;
	}
	if (vec3_equal(coll_with_pos, glm::vec3(-100.0f))) coll_with_pos = curr_pos;
	for (int i{}; i < all_handles->size(); ++i)
	{
		npc_handle c_mem{*all_handles->at(i)};
		if (c_mem.name == name) continue;
		if (c_mem.name == obj_name)
		{
			if (vec3_equal(coll_with_pos, c_mem.curr_pos))
			{
				return all_handles->at(i);
			}
			else return nullptr;
		}
	}
	std::cerr << "[*] Warning: npc_handle::check_for_collision - NPC with name " << obj_name << " doesn't exist\n";
	return nullptr;
}

bool npc_handle::check_for_collision_by_type(const glm::vec3& pos, std::string type)
{
	std::vector<npc_handle*>& npc_check{*(get_handles_by_type(type))};
	for (int i{}; i < npc_check.size(); ++i)
	{
		if (vec3_equal(pos, npc_check[i]->curr_pos)) return true;
	}
	return false;
}


glm::vec3 npc_handle::gen_rand_pos(bool ignore_dead = false)
{
	if (!all_handles)
	{
		std::cerr << "[*] Warning: npc_handle::gen_rand_pos() - all_handles pointer not set. Position not set.\n";
		return glm::vec3(0.0f);
	} else if (!map_grid->size())
	{
		std::cout << "[D] nh::gen_rand_pos() - map_grid size is zero; Setting all poses to zero\n";
		curr_pos = glm::vec3(0.0f);
		target_pos = curr_pos;
		last_pos = curr_pos;
		return curr_pos;
	}
	glm::vec3 rand_pos{};
	std::vector<glm::vec3> rand_poses{*map_grid};
	for (int i{static_cast<int>(rand_poses.size())}; i >= 0; --i)
	{
		for (int a{}; a < all_handles->size(); ++a)
		{
			npc_handle& c_mem{*all_handles->at(a)};
			if (&c_mem == this || (c_mem.is_dead && ignore_dead)) continue;
			if (vec3_equal(rand_poses[i], c_mem.curr_pos, 1.0f))
			{
				rand_poses.erase(rand_poses.begin() + i);
				break;
			}
		}
	}
	rand_pos = choose_rand(rand_poses, rand());
	//printf("Generated rand_pos for %s: %s; But is it free?: %i\n", name.c_str(), vec_to_str(rand_pos, 2).c_str(), !pos_taken(rand_pos, "slime, player, boost, bat, chest"), 1.0f);
	curr_pos = rand_pos;
	target_pos = curr_pos;
	last_pos = curr_pos;
	return rand_pos;
}

struct render_object
{
	int square_VAO{};
	glm::vec3 pos{};
	float rot{};
	glm::vec3 color{};
	glm::vec3 scale{};
	unsigned int gl_texture{};
	flex_shader* f_shader{};
	std::string render_type{"color"};
	bool delete_after_render{true};
	float transparency{1.0f};
	std::string name{"name_not_set"};
	npc_handle* obj_handle{};
	bool static_transform{};
	bool dynamic_scale{};
};


render_object get_fade_rinfo(const glm::vec3& pos, const glm::vec3& scale, flex_shader* fade_shader, int gl_texture, int square_VAO)
{
	render_object rinfo_fade{};
	rinfo_fade.color = glm::vec3{1.0f, 0.0f, 0.0f};
	rinfo_fade.f_shader = fade_shader;
	rinfo_fade.gl_texture = gl_texture;
	rinfo_fade.pos = pos;
	rinfo_fade.scale = scale;
	rinfo_fade.square_VAO = square_VAO;
	rinfo_fade.render_type = "fade";
	rinfo_fade.scale[0] *= 1.8f;
	return rinfo_fade;
}

void npc_handle::draw(const glm::mat4& m_projection)
{
	if (!square_VAO)
	{
		std::cerr << "[*] npc_handle::draw(): Error - bad or unset square_VAO\n";
		return;
	}
	if (!all_shaders->size())
	{
		std::cerr << "[*] npc_handle::draw(): Error - bad or unset p_shader. Did you create it with npc_handle::create_shader()?\n";
		return;
	}
	if (!max_morder)
	{
		std::cerr << "[*] Error: npc_handle::draw - max_morder variable is unset; Eiting\n";
		return;
	}
	if (is_dead)
	{
		if (*curr_morder == moving_order && is_npc) inc_order();
		return;
	}
	char key_dir{td::get_key_dir(window)};
	if (key_dir && !last_key_dir)
	{
		lb_info.direction = key_dir;
		lb_info.set_time = glfwGetTime();
	}
	if (key_dir != 0 && vec3_equal(curr_pos, target_pos) && last_key_dir == 0 && *curr_morder == moving_order && !is_npc && !is_dead) //Player handle
	{
		if(key_dir == 'r')
		{
			target_pos = curr_pos;
			target_pos[0] += moving_step;
			if (vec3_scale[0] < 0) vec3_scale[0] *= -1.0f;
		} else if (key_dir == 'l')
		{
			target_pos = curr_pos;
			target_pos[0] -= moving_step;
			if (vec3_scale[0] > 0) vec3_scale[0] *= -1.0f;
		} else if (key_dir == 'd')
		{
			target_pos = curr_pos;
			target_pos[1] -= moving_step;
		} else if (key_dir == 'u')
		{
			target_pos = curr_pos;
			target_pos[1] += moving_step;
		}
		moving_vec = target_pos - curr_pos;
		moving_dir = td::mvec_to_dir(moving_vec);
		new_pos_time = glfwGetTime();
	} else if (key_dir == 0 && npc_type == "player" && *curr_morder == moving_order && vec3_equal(curr_pos, target_pos) && !is_dead) // Player handle - Early move
	{
		if ( glfwGetTime() - lb_info.set_time > lb_info.allowed_diff)
		{
			lb_info.direction = 0;
		} else if (lb_info.direction != 0)
		{
			//std::cout << "Early move\n";
			target_pos = curr_pos + td::dir_to_vec(lb_info.direction, moving_step);
			moving_vec = target_pos - curr_pos;
			moving_dir = td::mvec_to_dir(moving_vec);
			if (moving_dir == 'r' && vec3_scale[0] < 0.0f) vec3_scale[0] *= -1.0f;
			new_pos_time = glfwGetTime();
			lb_info.direction = 0;
		}
	} 
	else if ( is_npc && moving_order == *curr_morder && vec3_equal(curr_pos, target_pos, 0.01f) && !is_dead ) // Generating new target position for all npc handles
	{
		type_behaviour();
		if (vec3_equal(curr_pos, target_pos)) 
		{
			inc_order();
		}
		moving_vec = target_pos - curr_pos;
		if (npc_type != "boost") moving_dir = td::mvec_to_dir(moving_vec);
		new_pos_time = glfwGetTime();
	}
	bool npc_moved{vec_moved(curr_pos, target_pos, moving_vec)};
	if (!npc_moved && npc_type != "boost")
	{
		float change{static_cast<float>(glfwGetTime()) - new_pos_time};
		curr_pos += (change*change + change)*0.7f * moving_vec;
		//npc_moved = vec_moved(curr_pos, target_pos, moving_vec);
	} 
	else if ( npc_moved && !vec3_equal(curr_pos, last_pos, 0.00000001f) && vec3_equal(curr_pos, target_pos, 0.000000001f) && npc_type != "boost" && *curr_morder == moving_order && !z_pos_switch)
	{
		curr_pos = target_pos;
		if (!pos_on_grid(curr_pos, 0.1f))
		{
		 	//std::cout << "Npc with name " << name << " was killed by bounds\n"; //Debug
			death_time = glfwGetTime();
			is_dead = true;
		}
		boosted_before = boosted;
		boosted = false;
		npc_handle* npc_boost{get_handle_by_name("npc_boost")};
		if (vec3_equal(curr_pos, npc_boost->curr_pos, 1.0f) && !boosted_before && !boosted)
		{
			target_pos = target_pos + td::dir_to_vec(npc_boost->moving_dir, moving_step);
			moving_vec = target_pos - curr_pos;
			moving_dir = td::mvec_to_dir(moving_vec);
			new_pos_time = glfwGetTime();
			npc_boost->moving_dir = td::turn_dir(npc_boost->moving_dir);
			while (!pos_on_grid(npc_boost->curr_pos + td::dir_to_vec(npc_boost->moving_dir, moving_step), 0.1f)) npc_boost->moving_dir = td::turn_dir(npc_boost->moving_dir);
			boosted = true;
			if (npc_type == "player" && boosted)
			{
				if (moving_dir == 'r' && vec3_scale[0] < 0.0f) vec3_scale[0] *= -1.0f;
			}
		} 
		end_move();	
		//std::cout << "Ended move for: " << name << "; will increase order: " << (!boosted && !reset_this_turn) << "\n"; //Debug
		if (!boosted && !reset_this_turn) inc_order();
		else if (reset_this_turn) reset_this_turn = false;
	}
	if (z_pos_switch && vec3_equal(curr_pos, target_pos)) z_pos_switch = 0.0f;
	if (vec3_equal(curr_pos, last_pos) && !boosted && boosted_before) boosted_before = false;
	if (npc_type == "portal" && *curr_morder == moving_order) inc_order();
	if (boosted_before && npc_type == "slime")
	{
		glm::vec3 facing_pos(curr_pos + td::dir_to_vec(moving_dir, moving_step));
		if (pos_taken(facing_pos, "grid"))
		{
			turn_till_on_grid(rand()%2);
		}
	}

	npc_handle& npc_portal{*get_handle_by_name("npc_portal")};
	if (npc_portal.is_dead)
	{
		int count_alive{};
		std::string enemies{"bat, slime"};
		std::vector<std::string> enemies_vec;
		split_str(enemies, &enemies_vec);
		for (int i{}; i < all_handles->size(); ++i)
		{
			if (!all_handles->at(i)->is_dead && index_vec(all_handles->at(i)->npc_type, enemies_vec) != -1)
			{
				++count_alive;
			}
		}
		if (!count_alive)
		{
			//std::cout << "[I] Activating portal!\n"; //Debug
			npc_portal.is_dead = false;
			npc_portal.gen_rand_pos();
		}
	}

	curr_pos[2] = z_pos + z_pos_switch;
	if (boosted || boosted_before)
	{
		curr_pos[2] += 0.2f;
	}
	if (*curr_morder == moving_order /*|| boosted_before*/ || !vec3_equal(last_pos, curr_pos))
	{
		curr_pos[2] += z_pos_add;
	}
	flex_shader& shader{*all_shaders->at(0)};
	render_object& curr_rinfo{*(new render_object{})};
	curr_rinfo.render_type = "color";
	curr_rinfo.pos = curr_pos;
	curr_rinfo.f_shader = &shader;
	curr_rinfo.color = color;
	curr_rinfo.square_VAO = square_VAO;
	curr_rinfo.scale = vec3_scale * scale;
	curr_rinfo.obj_handle = this;
	p_rinfo->push_back(&curr_rinfo);
	if (shader_level >= shader_texture)
	{
		if (!gl_texture)
		{
			std::cerr << "[*]Error: npc_handle::draw() - npc_handle::gl_texture is unset\n";
		}
		render_object& tex_rinfo{*(new render_object)};
		tex_rinfo.render_type = "texture";
		flex_shader& tex_shader{*((*all_shaders)[1])};
		tex_rinfo.f_shader = &tex_shader;
		tex_rinfo.gl_texture = tex_shader.main_texture;
		if (gl_texture) tex_rinfo.gl_texture = gl_texture;
		curr_pos[2] = curr_pos[2] + 0.01f;
		float rotation{td::dir_tor(moving_dir)};
		tex_rinfo.pos = curr_pos + td::fix_rotation(local_texture_pos, rotation);
		if (!lock_tex_rot) tex_rinfo.rot = rotation;
		tex_rinfo.scale = vec3_scale * scale;
		tex_rinfo.square_VAO = square_VAO;
		tex_rinfo.obj_handle = this;
		p_rinfo->push_back(&tex_rinfo);
	}
	if (npc_type == "slime" && shader_level >= shader_fade)
	{
		float rotation{td::dir_tor(moving_dir)};
		rinfo_fade->pos = curr_pos + td::fix_rotation(fade_local_pos, rotation);
		rinfo_fade->pos[2] += 0.01f;
		rinfo_fade->rot = rotation;
		rinfo_fade->f_shader = all_shaders->at(2);
		rinfo_fade->delete_after_render = false;
		rinfo_fade->scale = glm::vec3(30.0f);
		rinfo_fade->scale[0] *= 7.0f;
		rinfo_fade->scale[2] = 1.0f;
		rinfo_fade->render_type = "fade";
		rinfo_fade->obj_handle = this;
		p_rinfo->push_back(rinfo_fade);
	}
	last_grid_pos = grid_pos;
	last_key_dir = key_dir;
	last_moving_dir = moving_dir;
	last_pos = curr_pos;
}


unsigned int gen_texture(std::string texture_path, bool enable_opacity = false)
{
	int img_w{}, img_h{}, img_ch{};
	unsigned char* data{stbi_load(texture_path.c_str(), &img_w, &img_h, &img_ch, 0)};
	if (!data)
	{
		std::cerr << "[*] Error in gen_texture(): bad path: " << texture_path << "\n";
		return 0;
	}
	//std::cout << "Amount of channels in file: " << texture_path << ": " << img_ch << "\n"; //Debug
	unsigned int gl_texture{};
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &gl_texture);
	glBindTexture(GL_TEXTURE_2D, gl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int channels_size{GL_RGB};
	//if (img_ch == 4) channels_size = GL_RGBA;
	if (enable_opacity) channels_size = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return gl_texture;
}

npc_handle::npc_handle(int starting_grid_pos, unsigned int in_square_VAO, bool in_is_npc, const float in_moving_step, void* in_all_handles, int* in_max_morder, std::vector<render_object*>* in_p_rinfo, std::vector<flex_shader*>* in_all_shaders, const std::vector<glm::vec3>* in_map_grid, std::string npc_name = "npc_name_not_set")
{
	if (!in_square_VAO) std::cerr << "[*] npc_handle: Warning - bad square_VAO\n";
	if (!in_all_handles)
	{
		std::cerr << "[*] Error: npc_handle::npc_handle - in_all_handles variable is zero\n";
		return;
	}
	if (!in_p_rinfo)
	{
		std::cerr << "[*] Error: npc_handle::npc_handle() - in_p_rinfo is zero\n";
		return;
	}
	if (!in_all_shaders)
	{
		std::cerr << "[*] Error: npc_handle::npc_handle() - in_all_handles pointer is zero\n";
		return;
	}
	if (!in_map_grid)
	{
		std::cerr << "[*] Error: npc_handle::npc_handle() - in_map_grid pointer is zero\n";
		return;
	}
	map_grid = in_map_grid;
	all_handles = reinterpret_cast<std::vector<npc_handle*>*>(in_all_handles);
	p_rinfo = in_p_rinfo;
	if (starting_grid_pos == npc_handle::rand_pos)
	{
		gen_rand_pos();	
	} else curr_pos = get_grid_pos(starting_grid_pos);
	if (!in_max_morder)
	{
		std::cerr << "[*] Error: npc_handle::npc_handle - in_max_morder pointer is zero\n";
		return;
	}
	all_shaders = in_all_shaders;
	max_morder = in_max_morder;
	++*max_morder;
	name = npc_name;
	target_pos = curr_pos;
	moving_order = all_handles->size();
	square_VAO = in_square_VAO;
	last_grid_pos = grid_pos;
	last_pos = curr_pos;
	is_npc = in_is_npc;
	moving_step = in_moving_step;
	glm::vec3 f_scale{126.0f};
	f_scale[0] *= 0.2f;
	if (all_shaders->size() > 2){
		rinfo_fade = new render_object{get_fade_rinfo(curr_pos, f_scale, all_shaders->at(2), 0, square_VAO)};
		rinfo_fade->delete_after_render = false;
		fade_local_pos[2] += moving_order * 0.0001f;
	}
	turn_till_on_grid();
	all_handles->push_back(this);
	
}



npc_handle* npc_handle::get_player_handle()
{
	if (!all_handles)
	{
		std::cerr << "[*] Error: npc_handle::get_player_handle() - all_handles pointer is zero\n";
		return nullptr;
	}
	for (int i{}; i < all_handles->size(); ++i)
	{
		if (all_handles->at(i)->npc_type == "player") return all_handles->at(i);
	}
	return nullptr;
}

player_params* npc_handle::get_player_params()
{
	if (this->name == "player")
	{
		std::cerr << "[*] Warning: npc_handle::get_player_params() - You are already a player\n";
		return nullptr;
	}
	return reinterpret_cast<player_params*>(get_player_handle()->params);
}

bool npc_handle::pos_on_grid(const glm::vec3 pos, const float allowed_error)
{
	if (!map_grid)
	{
		std::cerr << "[*]Error: npc_handle::pos_on_grid - map_grid pointer is not set\n";
		return false;
	}
	for (int i{}; i < map_grid->size(); ++i)
	{
		if (vec3_equal(pos, map_grid->at(i), allowed_error)) return true;
	}
	return false;
}

struct input_handle_vars
{
	GLFWwindow* window{};
	char last_key{};
	npc_handle* npc_player{};
};

void input_handle(input_handle_vars& ih_vars)
{
	float change_speed{1.0f * delta_time};
	GLFWwindow* window{ih_vars.window};
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, npc_handle::reset_score);
	} else if (glfwGetKey(window, GLFW_KEY_R))
	{
		if (!ih_vars.last_key && vec3_equal(ih_vars.npc_player->target_pos, ih_vars.npc_player->curr_pos)) ih_vars.npc_player->reset(true);
		ih_vars.last_key = 'r';
	} else ih_vars.last_key = 0;
	if (glfwGetKey(window, GLFW_KEY_KP_4))
	{
		v3_dumm[0] += change_speed;
		if (v3_dumm[0] > dumm_limit_max) v3_dumm[0] = dumm_limit_max;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_1))
	{
		v3_dumm[0] -= change_speed;
		if (v3_dumm[0] < dumm_limit_min) v3_dumm[0] = dumm_limit_min;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_5))
	{
		v3_dumm[1] += change_speed;
		if (v3_dumm[1] > dumm_limit_max) v3_dumm[1] = dumm_limit_max;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_2))
	{
		v3_dumm[1] -= change_speed;
		if (v3_dumm[1] < dumm_limit_min) v3_dumm[1] = dumm_limit_min;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6))
	{
		v3_dumm[2] += change_speed;
		if (v3_dumm[2] > dumm_limit_max) v3_dumm[2] = dumm_limit_max;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_3))
	{
		v3_dumm[2] -= change_speed;
		if (v3_dumm[2] < dumm_limit_min) v3_dumm[2] = dumm_limit_min;
	}
}


void flex_shader::fset_mat4(std::string u_name, glm::mat4 matrix)
{
	int uniform_pos{glGetUniformLocation(gl_id, u_name.c_str())};
	if (uniform_pos == -1)
	{
		std::cerr << "[*] flex_shader: Error looking for uniform with name: " << u_name << "\n";
		return;
	}
	glUniformMatrix4fv(uniform_pos, 1, GL_FALSE, glm::value_ptr(matrix));
}

void flex_shader::fset_vec3(std::string u_name, glm::vec3 vec)
{
	int uniform_pos{glGetUniformLocation(gl_id, u_name.c_str())};
	if (uniform_pos == -1)
	{
		std::cerr << "[*] flex_shader: Error looking for uniform with name: " << u_name << "; in shader named: " << name <<"\n";
		return;
	}
	glUniform3fv(uniform_pos, 1,  glm::value_ptr(vec));
}

void flex_shader::fset_int(std::string u_name, int set_value)
{
	int un{glGetUniformLocation(gl_id, u_name.c_str())};
	if (un == -1) 
	{
		std::cerr << "[*] flex_shader: Error finding uniform with name " << u_name << "\n";
	}
	glUniform1i(un, set_value);
}

void flex_shader::fset_uniform(std::string u_name, float set_value)
{
	int uniform_green{glGetUniformLocation(gl_id, u_name.c_str())};
	if (uniform_green == -1) 
	{
		std::cerr << "[*] flex_shader: Error finding uniform with name " << u_name << "\n";
	}
	glUniform1f(uniform_green, set_value);
}


flex_shader::flex_shader(std::string vs_path, std::string fs_path, std::string in_name = "flex_shader_name_not_set")
{
	std::string str_vs_source{simple_read(vs_path)};
	std::string str_fs_source{simple_read(fs_path)};
	const char* vs_source{str_vs_source.c_str()};
	const char* fs_source{str_fs_source.c_str()};
	unsigned int id{glCreateProgram()};
	int success{};
	char info_log[512]{};
	unsigned int gl_fs{glCreateShader(GL_FRAGMENT_SHADER)};
	unsigned int gl_vs{glCreateShader(GL_VERTEX_SHADER)};
	glShaderSource(gl_vs, 1, &vs_source, 0);
	glCompileShader(gl_vs);
	glGetShaderiv(gl_vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(gl_vs, 512, 0, info_log);
		std::cerr << "[*] flex_shader error: error compiling vertex shader: " << info_log << "\n";
		return; 
	}
	glShaderSource(gl_fs, 1, &fs_source, 0);
	glCompileShader(gl_fs);
	glGetShaderiv(gl_fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(gl_fs, 512, 0, info_log);
		std::cerr << "[*] flex_shader error: error compiling fragment shader: " << info_log << "\n";
		return;
	}
	glAttachShader(id, gl_vs);
	glAttachShader(id, gl_fs);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, 0, info_log);
		std::cerr << "[*] flex_shader error: error linking program: " << info_log << "\n";
		return;
	}
	glDeleteShader(gl_fs);
	glDeleteShader(gl_vs);
	gl_id = id;
	name = in_name;
	return;
}


glm::vec3 local_coords(glm::vec3 vec)
{
	vec[0] += 1.0f;
	vec[1] += 1.0f;
	
	int biggest_coord{scr_w};
	int smallest_coord{scr_h};
	if (scr_h > scr_w) 
	{
		biggest_coord = scr_h;
		smallest_coord = scr_w;
	}
	
	float half_w{scr_w/2.0f}, half_h{scr_h/2.0f};
	glm::vec3 result{};
	result[0] = vec[0]*half_w;
	result[1] = vec[1] * half_h;
	result[2] = vec[2];
	return result;
}


void draw_grid(const glm::vec3& grid_pos, const glm::vec3& grid_color, int square_vao, flex_shader& flat_shader, std::vector<render_object>* grid_objects, std::vector<render_object*>* all_ren)
{
	if (!grid_objects->size())
	{
		render_object& curr_render{*(new render_object)};
		curr_render.f_shader = &flat_shader;
		curr_render.square_VAO = square_vao;
		curr_render.render_type = "color";
		curr_render.delete_after_render = false;
		curr_render.color = grid_color;
		curr_render.transparency = 1.0f;
		int i{};
		constexpr float border_width{8.0f};
		constexpr float border_length{400.0f + border_width};
		constexpr float pos_z{-0.9f};
		
		curr_render.pos = glm::vec3(200.0f, 300.0f,pos_z)+grid_pos;
		curr_render.scale = glm::vec3(border_width, border_length, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(600.0f, 300.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_width, border_length, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(400.0f, 500.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_length, border_width, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);
		
		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(400.0f, 100.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_length, border_width, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(333.0f, 300.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_width, border_length, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(466.0f, 300.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_width, border_length, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(400.0f, 367.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_length, border_width, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);

		curr_render = *(new render_object{curr_render});
		curr_render.pos = glm::vec3(400.0f, 234.0f,pos_z) + grid_pos;
		curr_render.scale = glm::vec3(border_length, border_width, 1.0f);
		curr_render.name = "grid_object_" + std::to_string(++i);
		grid_objects->push_back(curr_render);
	}
	for (int i{}; i < grid_objects->size(); ++i)
	{
		(*grid_objects)[i].color = grid_color;
		all_ren->push_back(&grid_objects->at(i));
	}
}

class text_controller
{
public:
	text_controller(const glm::vec3, const unsigned int, flex_shader*, std::string);
	void draw(std::vector<render_object*>*);
	glm::vec3 position{};
	std::string text;
	std::string last_text;
	flex_shader* texture_shader{};
	int square_VAO{};
	const char chars_numbers[10]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	unsigned int number_textures[10]{};
	float font_scale{5.0f};
	float time_of_change{};
	std::vector<render_object> number_objects;
};

text_controller::text_controller(const glm::vec3 starting_position, const unsigned int in_square_VAO, flex_shader* in_texture_shader, std::string path_to_nums)
{
	if (!in_texture_shader) 
	{
		std::cerr << "[*]Error: text_controller::text_controller - in_texture_shader pointer is 0\n";
		return;
	}
	position = starting_position;
	square_VAO = in_square_VAO;
	texture_shader = in_texture_shader;
	for (int i{}; i < 10; ++i)
	{
		number_textures[i] = gen_texture(path_to_nums + std::to_string(i) + ".png", true);
		render_object curr_render{};
		curr_render.gl_texture = number_textures[i];
		curr_render.render_type = "texture";
		curr_render.name = "number_object_" + std::to_string(i);
		curr_render.square_VAO = square_VAO;
		curr_render.f_shader = texture_shader;
		curr_render.delete_after_render = false;
		number_objects.push_back(curr_render);
	}
}

void text_controller::draw(std::vector<render_object*>* all_ren)
{
	if (!text.size() || !all_ren)
	{
		std::cerr << "[*] Warning: text_controller::draw() - text or all_ren is zero. Returning.\n";
		return;
	}
	for (int i{}; i < text.size(); ++i)
	{
		char mem_char{text[i]};
		if (!obj_in_arr(mem_char, reinterpret_cast<const char*>(chars_numbers), 10))
		{
			std::cerr << "[*]Error: text_controller::draw() - current version supports only numbers\n";
			return;
		}
	}
	if (text != last_text)
	{
		//std::cout << "Last text: " << last_text << "; curr_text: " << text << "\n";
		time_of_change = glfwGetTime();
	}
	glm::vec3 char_pos{position};
	float change{static_cast<float>(glfwGetTime() - time_of_change)};
	float dynamic_scale{-(change*change*0.7f) + 1.4f};
	if (dynamic_scale < 1.0f) dynamic_scale = 1.0f;
	glm::vec3 z_offset_vec{0.0f, 0.0f, 0.8f};
	for (int i{}; i < text.size(); ++i)
	{
		int curr_texture{text[i] - '0'};
		render_object& curr_render{*(new render_object{number_objects[curr_texture]})};
		curr_render.pos = char_pos + z_offset_vec;
		curr_render.scale = glm::vec3(7.0f, 10.0f, 0.0f) * font_scale * dynamic_scale;
		curr_render.delete_after_render = true;
		curr_render.static_transform = false;
		all_ren->push_back(&curr_render);
		char_pos[0] += font_scale*7.5f;
		z_offset_vec[2] += 0.0001f;
	}
	last_text = text;
}


void sort_render_object(std::vector<render_object*>& rinfo)
{
	bool sorted{};
	while (!sorted){
		sorted = true;
		for (int i{}; i < rinfo.size(); ++i)
		{
			for (int a{i+1}; a < rinfo.size(); ++a)
			{
				if (rinfo[i]->pos[2] > rinfo[a]->pos[2])
				{
					render_object* cache{rinfo[i]};
					rinfo[i] = rinfo[a];
					rinfo[a] = cache;
					sorted = false;
					break;
				}
			}
		}
	}
}

void fix_fade_objs(std::vector<render_object*>& rinfo)
{
	for (int i{}; i < rinfo.size(); ++i)
	{
		if (rinfo[i]->render_type == "fade")
		{
			rinfo[i]->pos[2] += 0.01f * i;
		}
	}
}

void render_handle(std::vector<render_object*>& rinfo, const glm::mat4& m_projection, const float g_scr_size)
{
	sort_render_object(rinfo);
	while (rinfo.size())
	{
		//if (rinfo[0]->name != "name_not_set") std::cout << "[D] Now working with render_object named: " << rinfo[0]->name << "\n";
		//if (rinfo[0]->obj_handle) std::cout << "[D] it's obj name: " << rinfo[0]->obj_handle->name << "\n";
		if (!rinfo[0]->f_shader) 
		{
			std::cerr << "[*]Error: render_handle() - f_shader is not set\n";
			continue;
		}
		glBindVertexArray(rinfo[0]->square_VAO);
		flex_shader& shader{*rinfo[0]->f_shader};
		glUseProgram(shader.gl_id);

		glm::mat4 m_model{1.0f};
		const glm::vec3 starting_middle{400.0f, 300.0f, 0.0f};
		const glm::vec3 scr_middle{scr_w/2.0f, scr_h/2.0f, 0.0f};
		glm::vec3 curr_pos{};
		//if (rinfo[0]->static_transform) curr_pos = td::center_coord(rinfo[0]->pos, starting_middle, scr_middle, 1.0f);
		if (rinfo[0]->static_transform) curr_pos = td::center_coord(rinfo[0]->pos, scr_middle, starting_middle, 1.0f);
		//else curr_pos = td::center_coord(rinfo[0]->pos, starting_middle, scr_middle, g_scr_size);
		else curr_pos = td::center_coord(rinfo[0]->pos, scr_middle, starting_middle, g_scr_size);
		curr_pos[2] = rinfo[0]->pos[2];
		m_model = glm::translate(m_model, curr_pos);
		if (rinfo[0]->rot) m_model = glm::rotate(m_model, glm::radians(rinfo[0]->rot), glm::vec3(0.0f, 0.0f, -1.0f));
		if (!rinfo[0]->static_transform || rinfo[0]->dynamic_scale) m_model = glm::scale(m_model, rinfo[0]->scale * g_scr_size);
		else if (rinfo[0]->static_transform) m_model = glm::scale(m_model, rinfo[0]->scale);
		if (rinfo[0]->render_type == "color") 
		{
			
			shader.fset_vec4("color", glm::vec4(rinfo[0]->color, rinfo[0]->transparency));
		}
		else if (rinfo[0]->render_type == "texture" || rinfo[0]->render_type == "fade")
		{
			glActiveTexture(GL_TEXTURE0); if (!rinfo[0]->gl_texture)
			{
				glBindTexture(GL_TEXTURE_2D, rinfo[0]->f_shader->main_texture);
			} else  
			glBindTexture(GL_TEXTURE_2D, rinfo[0]->gl_texture);
			shader.fset_int("main_texture", 0);
			if (rinfo[0]->render_type == "fade")
			{
				shader.fset_vec3("color", rinfo[0]->color);
			}
		}
		else 
		{
			std::cerr << "[*] Error: render_handle - bad render_type: " << rinfo[0]->render_type << "\n";
		}
		shader.fset_mat4("m_projection", m_projection);
		shader.fset_mat4("m_model", m_model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		if (rinfo[0]->delete_after_render) 
		{
			delete rinfo[0];
		}
		rinfo.erase(rinfo.begin());
	}
}

void render_basic_texture(const unsigned int square_VAO, const glm::vec3& position, const glm::vec3& scale, flex_shader& texture_shader, unsigned int gl_texture, std::vector<render_object*>* robj_all)
{
	render_object& robj_curr{*(new render_object)};
	robj_curr.square_VAO = square_VAO;
	robj_curr.gl_texture = gl_texture;
	robj_curr.f_shader = &texture_shader;
	robj_curr.pos = position;
	robj_curr.scale =  scale;
	robj_curr.delete_after_render = true;
	robj_curr.render_type = "texture";
	//robj_curr.static_transform = true;
	robj_all->push_back(&robj_curr);
}

void set_time_frame(float* time_frames, float current_time, const int fbuff_size)
{
	int zero_mem{-1};
	for (int i{}; i < fbuff_size; ++i)
	{
		if (time_frames[i] == 0)
		{
			zero_mem = i;
			break;
		}
	}
	if (zero_mem != -1)
	{
		time_frames[zero_mem] = current_time;
		return;
	} 
	for (int i{fbuff_size-2}; i >= 0; --i)
	{
		time_frames[i+1] = time_frames[i];
	}
	time_frames[0] = current_time;
}

int get_fps(float* time_frames, const int fbuff_size)
{
	int zero_mem{-1};
	float total_time{};
	int total_frames{fbuff_size};
	for (int i{}; i < fbuff_size; ++i)
	{
		if (time_frames[i] == 0)
		{
			total_frames = i;
			break;
		}
		total_time += time_frames[i];
	}
	if (!total_frames) return 120;
	float medium_time{total_time/total_frames};
	if (total_frames < 5) return 111;
	return static_cast<int>(1/medium_time);
}


std::vector<glm::vec3>* gen_map_grid(const float moving_step, const glm::vec3 map_middle, const int grid_size)
{
	if (grid_size%2!=1)
	{
		std::cerr << "[*]Error: gen_map_grid() - grid size must be odd\n";
		return nullptr;
	}
	int ul_int{(grid_size-1)/2};
	glm::vec3 ul_pos{map_middle};
	ul_pos[0] -= moving_step * ul_int;
	ul_pos[1] += moving_step * ul_int;
	std::vector<glm::vec3>& result{*(new std::vector<glm::vec3>)};
	int line{};
	for (int i{}; i < grid_size*grid_size; ++i)
	{
		if (i%grid_size == 0 && i) ++line;
		glm::vec3 curr_pos{ul_pos};
		curr_pos[0] += moving_step*(i-line*grid_size);
		curr_pos[1] -= moving_step * line;
		result.push_back(curr_pos);
		//std::cout << "gen_map_grid: grid pos number " << i+1 << " is: " << vec_to_str(curr_pos, 2) << "\n"; //debug
	}
	return &result;
}

class control_handle
{
public:
	struct key_info
	{
		char ch_rep{};
		bool active{};
	};
	GLFWwindow* window{};
	std::vector<char> active_keys;
	std::vector<char> last_a_keys;
	glm::vec3 vec3{};
	float speed{75.0f};
	float z_speed{0.2f};
	std::vector<render_object*>* ren_objs;
	render_object square_obj{};
	bool render{};
	int* curr_morder;
	npc_handle* npc_player;
	control_handle(GLFWwindow* in_window, std::vector<render_object*>* in_ren_objs)
	{
		window = in_window;
		ren_objs = in_ren_objs;
	}
	void update_state()
	{
		if (!window)
		{
			std::cerr << "[*] Error: window pointer is zero\n";
			return;
		}
		last_a_keys = active_keys;
		active_keys.clear();
		for (int i{}; i < 10; ++i){
			if (glfwGetKey(window, GLFW_KEY_KP_0 + i))
			{
				active_keys.push_back('0'+i);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			active_keys.push_back('w');
		}
		if (glfwGetKey(window, GLFW_KEY_S))
		{
			active_keys.push_back('s');
		}
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			active_keys.push_back('a');
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			active_keys.push_back('d');
		}
		if (glfwGetKey(window, GLFW_KEY_T))
		{
			active_keys.push_back('t');
		}
		if (glfwGetKey(window, GLFW_KEY_G))
		{
			active_keys.push_back('g');
		}
		if (glfwGetKey(window, GLFW_KEY_E))
		{
			active_keys.push_back('e');
		}
		if (glfwGetKey(window, GLFW_KEY_C))
		{
			active_keys.push_back('c');
		}
	}
	bool get_state(char key)
	{
		auto found{std::find(active_keys.begin(), active_keys.end(), key)};
		if (found != active_keys.end()) return true;
		return false;
	}

	bool get_last_state(char key)
	{
		auto found{std::find(last_a_keys.begin(), last_a_keys.end(), key)};
		if (found != last_a_keys.end()) return true;
		return false;
	}
	void update_vec(const float delta_time)
	{
		auto found{std::find(active_keys.begin(), active_keys.end(), 'a')};
		auto found_last{std::find(last_a_keys.begin(), last_a_keys.end(), 'e')};
		if (found != active_keys.end())
		{
			vec3[0] -= speed * delta_time;
		}
		found = std::find(active_keys.begin(), active_keys.end(), 'd');
		if (found != active_keys.end())
		{
			vec3[0] += speed * delta_time;
		}
		found = std::find(active_keys.begin(), active_keys.end(), 'w');
		if (found != active_keys.end())
		{
			vec3[1] += speed * delta_time;
		}
		found = std::find(active_keys.begin(), active_keys.end(), 's');
		if (found != active_keys.end())
		{
			vec3[1] -= speed * delta_time;
		}
		found = std::find(active_keys.begin(), active_keys.end(), 't');
		if (found != active_keys.end())
		{
			vec3[2] += z_speed * delta_time;
		}
		found = std::find(active_keys.begin(), active_keys.end(), 'g');
		if (found != active_keys.end())
		{
			vec3[2] -= z_speed * delta_time;
		}
		if (get_state('e') && !get_last_state('e'))
		{
			render = !render;
			std::cout << "Render is now ";
			if (render) std::cout << "enabled\n";
			else std::cout << "disabled\n";
		}
		if (render)
		{
			square_obj.pos = vec3;
		}
	}
};

class level_editor
{
public:
	struct tile_info
	{
		glm::vec3 pos{};
		render_object square_renders[4]{};
	};
	control_handle* chandle{};
	mouse_handle* mhandle{};
	std::vector<npc_handle*>* all_handles{};
	std::vector<glm::vec3>* map_grid{};
	float moving_step{};
	float g_scr_size{};
	std::vector<tile_info> all_tiles;
	unsigned int square_VAO{};
	flex_shader* flat_shader{};
	std::vector<render_object*>* g_rinfo{};
	glm::vec3 scr_middle{};
	glm::vec3 init_scr_middle{};
	void debug(std::string message)
	{
		std::cout << "[D] level_editor: " << message << "\n";
		return;
	}
	void error(std::string message)
	{
		std::cerr << "[E] level_editor: " << message << "\n";
		return;
	}
	void set_info_scale_pos(const glm::vec3& pos, tile_info* p_curr_info)
	{
		if (!p_curr_info)
		{
			error("p_curr_info pointer is zero\n");
			return;
		}
		tile_info& curr_info(*p_curr_info);
		const float half_step{moving_step/2.0f};
		curr_info.square_renders[0].pos = glm::vec3(pos[0]+half_step, pos[1], 0.0f);
		curr_info.square_renders[1].pos = glm::vec3(pos[0]-half_step, pos[1], 0.0f);
		curr_info.square_renders[2].pos = glm::vec3(pos[0], pos[1]+half_step, 0.0f);
		curr_info.square_renders[3].pos = glm::vec3(pos[0], pos[1]-half_step, 0.0f);
		const float square_width{moving_step/18.0f};
		const float square_height{moving_step+square_width};
		curr_info.square_renders[0].scale = glm::vec3(square_width, square_height, 1.0f);
		curr_info.square_renders[1].scale = glm::vec3(square_width, square_height, 1.0f);
		curr_info.square_renders[2].scale = glm::vec3(square_height, square_width, 1.0f);
		curr_info.square_renders[3].scale = glm::vec3(square_height, square_width, 1.0f);
		return;
	}
	void gen_map_grid(const int grid_size = 3)
	{
		const int tiles_amount{grid_size*grid_size};
		int ul_int{(grid_size-1)/2};
		glm::vec3 ul_pos{400.0f, 300.0f, 0.0f};
		ul_pos[0] -= moving_step * ul_int;
		ul_pos[1] += moving_step * ul_int;
		int line{};
		for (int i{}; i < tiles_amount; ++i)
		{
			if (i%grid_size == 0 && i) ++line;
			glm::vec3 curr_pos{ul_pos};
			curr_pos[0] += moving_step*(i-line*grid_size);
			curr_pos[1] -= moving_step * line;
			//curr_pos = stick_to_grid(curr_pos);
			tile_info curr_info{};
			curr_info.pos = curr_pos;
			for (int a{}; a < 4; ++a)
			{
				curr_info.square_renders[a].square_VAO = square_VAO;
				curr_info.square_renders[a].f_shader = flat_shader;
				curr_info.square_renders[a].render_type = "color";
				curr_info.square_renders[a].delete_after_render = false;
			}
			set_info_scale_pos(curr_pos, &curr_info);
			map_grid->push_back(curr_pos);
			all_tiles.push_back(curr_info);
		}
	}
	level_editor(control_handle* in_chandle, mouse_handle* in_mhandle, std::vector<npc_handle*>* in_all_handles, std::vector<glm::vec3>* in_map_grid, const float in_moving_step, unsigned int in_square_VAO, flex_shader* in_flat_shader, std::vector<render_object*>* in_g_rinfo)
	{
		if (!in_chandle || !in_all_handles || !in_mhandle || !in_map_grid || !in_moving_step || !in_square_VAO || !in_flat_shader || !in_g_rinfo)
		{
			std::cerr << "[E] level_editor::level_editor - some or all of crucial pointers are zero\n";
			return;
		}
		chandle = in_chandle;
		mhandle = in_mhandle;
		all_handles = in_all_handles;
		map_grid = in_map_grid;
		moving_step = in_moving_step;
		square_VAO = in_square_VAO;
		flat_shader = in_flat_shader;
		g_rinfo = in_g_rinfo;
		init_scr_middle = glm::vec3(scr_w/2.0f, scr_h/2.0f, 0.0f);
	}
	glm::vec3 stick_to_grid(const glm::vec3& pos)
	{
		glm::vec3 rel_pos{pos - scr_middle};
		rel_pos /= g_scr_size;
		glm::vec3 result{rel_pos};
		int whole_x{static_cast<int>(abs(rel_pos[0])/moving_step)};
		float rem_x{abs(rel_pos[0]) - moving_step * whole_x};
		if (rem_x >= moving_step/2.0f) ++whole_x;
		if (rel_pos[0] <= 0.0f) whole_x *= -1;
		result[0] = whole_x * moving_step;

		int whole_y{static_cast<int>(abs(rel_pos[1])/moving_step)};
		float rem_y{abs(rel_pos[1]) - moving_step * whole_y};
		if (rem_y >= moving_step/2.0f) ++whole_y;
		if (rel_pos[1] <= 0.0f) whole_y *= -1;
		result[1] = whole_y * moving_step;

		result += scr_middle;
		return result;
	}
	bool pos_on_grid(const glm::vec3& pos, const float error = 0.000001f)
	{
		debug("map_grid size: " + std::to_string(map_grid->size()));
		for (int i{}; i < map_grid->size(); ++i)
		{
			if ( vec3_equal(pos, map_grid->at(i), error) )
			{
				return true;
			}
		}
		return false;
	}
	void process(const glm::vec3& tile_color, const float in_g_scr_size)
	{
		g_scr_size = in_g_scr_size;
		scr_middle = glm::vec3(scr_w/2.0f, scr_h/2.0f, 0.0f);
		if (chandle->get_state('7') && !chandle->get_last_state('7'))
		{
			debug("New tile request");
			glm::vec3 mouse_pos{glm::vec3(mhandle->get_coords(), 0.0f)};
			mouse_pos = stick_to_grid(mouse_pos);
			if (pos_on_grid(mouse_pos, 0.1f*g_scr_size))
			{
				debug("Pos " + vec_to_str(mouse_pos, 2) + " is already grid!");
				for (int i{}; i < map_grid->size(); ++i)
				{
					if (vec3_equal(mouse_pos, (*map_grid)[i]))
					{
						map_grid->erase(map_grid->begin()+i);
						break;
					}
				}
				bool tile_erased{};
				for (int i{}; i < all_tiles.size(); ++i)
				{
					if (vec3_equal(mouse_pos, all_tiles[i].pos, 2.0f))
					{
						all_tiles.erase(all_tiles.begin()+i);
						tile_erased = true;
						break;
					}
				}
				if (tile_erased) debug("Sucessfuly deleted a tile");
				else debug("Wasn't able to find tile_info in all_tiles vec");
			} else 
			{
				debug("Pos " + vec_to_str(mouse_pos,2) + " is NOT grid.\n Creating new tile");
				tile_info curr_info{};
				curr_info.pos = mouse_pos;
				for (int i{}; i < 4; ++i)
				{
					curr_info.square_renders[i].square_VAO = square_VAO;
					curr_info.square_renders[i].f_shader = flat_shader;
					curr_info.square_renders[i].render_type = "color";
					curr_info.square_renders[i].delete_after_render = false;
				}
				set_info_scale_pos(mouse_pos, &curr_info);
				all_tiles.push_back(curr_info);
				map_grid->push_back(mouse_pos);
			}
			
		}
		for (int i{}; i < all_tiles.size(); ++i)
		{
			for (int a{}; a < 4; ++a)
			{
				all_tiles[i].square_renders[a].color = tile_color;
				g_rinfo->push_back(&all_tiles[i].square_renders[a]);
			}
		}
	}
};

float get_game_size(int scr_w, int scr_h)
{
	float start_ratio{start_scr_w/static_cast<float>(scr_h)};
	float curr_ratio{scr_w/static_cast<float>(scr_h)};
	float curr_size{};
	int start_size{start_scr_w*start_scr_h};
	if (curr_ratio > start_ratio)
	{
		scr_w = scr_h*start_ratio;
		curr_size = scr_h * scr_w;
		return curr_size/start_size;
	}
	scr_h = scr_w/start_ratio;
	curr_size = scr_h * scr_w;
	return curr_size/start_size;
}

int read_high_score(std::string path)
{
	std::string read{simple_read(path)};
	return atoi(read.c_str());
}

void display_dead(glm::vec3 pos, npc_handle* npc_player, unsigned int skeleton_texture, std::vector<render_object*>* robj, flex_shader* tex_shader, int VAO)
{
	constexpr float max_size{502.0f};
	static unsigned int restart_texture{gen_texture("./textures/restart.png")};
	static float last_death_time{};
	static bool render_hs{};
	static unsigned int hs_texture{gen_texture("./textures/highscore.png")};
	if (npc_player->is_dead)
	{
		float change{static_cast<float>(glfwGetTime() - npc_player->death_time)};
		float size{change*300.0f + change*change*300.0f + 10.0f};
		if (size > max_size) size = max_size;
		render_object& curr_render{*(new render_object{})};
		curr_render.render_type = "texture";
		curr_render.delete_after_render = true;
		curr_render.gl_texture = skeleton_texture;
		curr_render.scale = glm::vec3(size);
		curr_render.pos = pos;
		curr_render.f_shader = tex_shader;
		curr_render.static_transform = false;
		curr_render.color = glm::vec3(0);
		curr_render.square_VAO = VAO;
		robj->push_back(&curr_render);

		render_basic_texture(VAO, glm::vec3(400.0f, 552.0f, 1.0f), glm::vec3(30.0f, 8.0f, 1.0f)*6.0f, *tex_shader, restart_texture, robj);
		if (npc_player->death_time != last_death_time)
		{
			int last_high_score{read_high_score("high_score.txt")};
			player_params& p_params{*reinterpret_cast<player_params*>(npc_player->params)};
			int curr_score{p_params.score};
			if (curr_score > last_high_score && !npc_player->debug_mode)
			{
				std::cout << "New high score: " << curr_score << "!\n";
				simple_write("high_score.txt", std::to_string(curr_score));
				render_hs = true;
			} else if (npc_player->debug_mode)
			{
				std::cout << "[I] Score is not being recorded; You are in the debug mode\n";
			}
			
			last_death_time = npc_player->death_time;
		}
		if (render_hs)
		{
			render_basic_texture(VAO, glm::vec3(400.0f, 48.0f, 1.0f), glm::vec3(44.0f, 14.0f, 1.0f)*5.3f, *tex_shader, hs_texture, robj);
		}
	} else render_hs = false;
}

int index_argv(std::string arg, const char** argv, int argv_size)
{
	for (int i{}; i < argv_size; ++i)
	{
		std::string curr_arg{argv[i]};
		if (arg == curr_arg) return i;
	}
	return -1;
}

int main(const int argc, const char** argv)
{
	const int debug_mode{index_argv("-d", argv, argc)};
	if (debug_mode != -1) std::cout << "[!] Debug mode is on.\n";
	srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	rand();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window{glfwCreateWindow(scr_w, scr_h, "Roaring Dungeon", 0, 0)};
	if (!window)
	{
		std::cerr << "Error creating window\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Error loading glad\n";
		glfwTerminate();
		return 1;
	}
	glViewport(0, 0, scr_w, scr_h);
	glfwSetFramebufferSizeCallback(window, resize_handle);
	float time_start{static_cast<float>(glfwGetTime())};
	mouse_handle m_handle(window);
	unsigned int basic_texture{gen_texture("./textures/green.png")};
	unsigned int arrow_texture{gen_texture("./textures/arrow.png")};
	unsigned int coin_texture{gen_texture("./textures/coin.png")};
	unsigned int fade_texture{gen_texture("./textures/triangle_gardient.png")};

	float triangle_vertices[]
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	unsigned int VAO{}, VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), reinterpret_cast<void*>(sizeof(float)*3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), reinterpret_cast<void*>(sizeof(float)*5));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glm::vec3 light_pos{0.0f, 1.0f, 1.0f};
	glm::vec3 light_color{1.0f, 1.0f, 1.0f};

	constexpr float moving_step{133.0f};
	constexpr float tile_scale{12.6f};
	int curr_morder{0};
	int max_morder{-1};

	unsigned int gl_no_texture{gen_texture("./textures/no_texture.png")};
	unsigned int slime_texture{gen_texture("./textures/slime.png")};
	unsigned int skeleton_texture{gen_texture("./textures/skeleton.png")};
	flex_shader flat_shader{"shaders/flat_color.vs", "shaders/flat_color.fs", "flat_color_shader"};
	flex_shader texture_shader{"shaders/texture_opacity.vs", "shaders/texture_opacity.fs", "opacity_texture_shader"};
	texture_shader.main_texture = gl_no_texture;
	flex_shader fade_shader{"shaders/texture_fade.vs", "shaders/texture_fade.fs"};
	fade_shader.main_texture = fade_texture;
	std::vector<flex_shader*> all_shaders;
	all_shaders.push_back(&flat_shader);
	all_shaders.push_back(&texture_shader);
	all_shaders.push_back(&fade_shader);
	int lowest_fps{100};
	constexpr glm::vec3 map_middle{400.0f, 300.0f, 0.0f};
	std::vector<glm::vec3> map_grid;


	std::vector<npc_handle*> all_handles;
	std::vector<render_object*> rinfo;

	control_handle c_handle(window, &rinfo);
	c_handle.curr_morder = &curr_morder;
	c_handle.square_obj.delete_after_render = false;
	c_handle.square_obj.scale = glm::vec3(126.0f);
	c_handle.square_obj.f_shader = &flat_shader;
	c_handle.square_obj.color = glm::vec3(0.5f);
	c_handle.square_obj.square_VAO = VAO;
	c_handle.square_obj.render_type = "color";
	c_handle.render = false;

	level_editor leditor{&c_handle, &m_handle, &all_handles, &map_grid, moving_step, VAO, &flat_shader, &rinfo};
	leditor.gen_map_grid();
	/*
	std::cout << "Map_grid poses:\n";
	for (int i{}; i < map_grid.size(); ++i) std::cout << vec_to_str(map_grid[i], 2) << "\n";
	*/
	player_params p_player{};
	npc_handle npc_player{npc_handle::rand_pos, VAO, false, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "player_npc"};
	c_handle.npc_player = &npc_player;
	npc_player.curr_morder = &curr_morder;
	npc_player.window = window;
	npc_player.color = glm::vec3(0xF0, 0x5D, 0x23)/256.0f;
	npc_player.npc_type = "player";
	npc_player.scale = 12.6f;
	npc_player.params = &p_player;
	npc_player.map_grid = &map_grid;
	npc_player.z_pos = 0.0f;
	npc_player.lb_info.allowed_diff = 0.15f;
	npc_player.shader_level = npc_handle::shader_texture;
	npc_player.gl_texture = gen_texture("./textures/player.png");
	npc_player.lock_tex_rot = true;
	npc_player.debug_mode = debug_mode;

	npc_handle npc_slime{npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "slime_npc"};
	npc_slime.window = window;
	npc_slime.color = glm::vec3{0.2f, 0.9f, 0.1f};
	npc_slime.npc_type = "slime";
	npc_slime.z_pos = 0.0f;
	npc_slime.curr_morder = &curr_morder;
	npc_slime.scale = 12.6f;
	npc_slime.map_grid = &map_grid;
	npc_slime.gl_texture = slime_texture;
	npc_slime.basic_color_transparent = true;
	npc_slime.lock_tex_rot = true;
	npc_slime.shader_level = npc_handle::shader_fade;
	
	npc_handle npc_slime2(npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "slime_npc2");
	npc_slime2.window = window;
	npc_slime2.color = glm::vec3{0.2f, 0.9f, 0.1f};
	npc_slime2.npc_type = "slime";
	npc_slime2.z_pos = 0.0f;
	npc_slime2.curr_morder = &curr_morder;
	npc_slime2.scale = 12.6f;
	npc_slime2.map_grid = &map_grid;
	npc_slime2.gl_texture = slime_texture;
	npc_slime2.basic_color_transparent = true;
	npc_slime2.lock_tex_rot = true;
	npc_slime2.shader_level = npc_handle::shader_fade;

	npc_handle npc_chest{npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "npc_chest"};
	npc_chest.npc_type = "chest";
	npc_chest.z_pos = -0.1f;
	npc_chest.curr_morder = &curr_morder;
	npc_chest.scale = 12.6f;
	npc_chest.color = glm::vec3(0x89, 0x1a, 0xcc)/256.0f;
	npc_chest.shader_level = npc_handle::shader_texture;
	npc_chest.gl_texture = gen_texture("textures/chest.png");
	npc_chest.lock_tex_rot = true;

	npc_handle npc_boost{npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "npc_boost"};
	npc_boost.npc_type = "boost";
	npc_boost.z_pos = -0.1f;
	npc_boost.curr_morder = &curr_morder;
	npc_boost.color = glm::vec3(0.855f, 0.396f, 0.7176f);
	npc_boost.gl_texture = gen_texture("./textures/boost_arrow.png");
	npc_boost.scale = 12.6f;
	npc_boost.local_texture_pos = glm::vec3(4.5f, 0.0f, 0.0f);
	npc_boost.shader_level = npc_handle::shader_texture;

	npc_handle npc_portal{npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "npc_portal"};
	npc_portal.npc_type = "portal";
	npc_portal.z_pos = -0.1f;
	npc_portal.curr_morder = &curr_morder;
	npc_portal.color = glm::vec3(0.15f, 0.28f, 1.0f);
	npc_portal.scale = 12.6f;
	npc_portal.is_static = true;
	npc_portal.is_dead = true;
	npc_portal.shader_level = npc_handle::shader_texture;
	npc_portal.gl_texture = gen_texture("./textures/portal.png");
	npc_portal.lock_tex_rot = true;

	npc_handle npc_bat{npc_handle::rand_pos, VAO, true, moving_step, &all_handles, &max_morder, &rinfo, &all_shaders, &map_grid, "npc_bat"};
	npc_bat.npc_type = "bat";
	npc_bat.z_pos = 0.0f;
	npc_bat.curr_morder = &curr_morder;
	npc_bat.gl_texture = gen_texture("./textures/bat.png");
	npc_bat.scale = 12.6f;
	npc_bat.moving_dir = td::rand_dir(rand());
	npc_bat.color = glm::vec3(0.3f, 0.0f, 0.0f);
	npc_bat.lock_tex_rot = true;
	npc_bat.basic_color_transparent = true;
	npc_bat.shader_level = npc_handle::shader_texture;

	text_controller text_cntrl{glm::vec3(214.0f, 550.0f, 0.0f), VAO, &texture_shader, "./numbers_textures/"};
	text_cntrl.text = "0";

	constexpr int fbuff_size{5};
	float time_frames[fbuff_size]{};
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float performance{};
	float worst_perf{};
	float best_perf{100000.0f};
	float last_time_start{};
	constexpr int notify_fps_drop{59}; //Notify if FPS drops lower than value in the console
	input_handle_vars ih_vars{};
	ih_vars.window = window;
	ih_vars.npc_player = &npc_player;

	render_object rinfo_fade{};
	rinfo_fade.color = glm::vec3{1.0f, 0.0f, 0.0f};
	rinfo_fade.f_shader = &fade_shader;
	rinfo_fade.gl_texture = fade_texture;
	rinfo_fade.pos = glm::vec3(90.0f, 200.0f, 0.0f);	
	rinfo_fade.scale = glm::vec3(100.0f);
	rinfo_fade.square_VAO = VAO;
	rinfo_fade.render_type = "fade";
	rinfo_fade.scale[0] *= 1.8f;
	rinfo_fade.delete_after_render = false;
	

	float g_scr_size{(static_cast<float>(scr_w)/scr_h)};
	std::vector<render_object> grid_objects;
	glm::vec3 clear_color{0xE9, 0x72, 0x4C};
	clear_color /= 256.0f;
	glm::vec3 grid_pos{0.0f, 0.0f, 0.0f};
	glm::vec3 grid_color{glm::vec3(0x26, 0x1C, 0x15)/256.0f};
	glm::vec3 coin_position{160.0f, 550.0f, 0.9f};
	glm::mat4 m_projection{1.0f};
	while (!glfwWindowShouldClose(window))
	{
		g_scr_size = get_game_size(scr_w, scr_h);
		
		time_start = static_cast<float>(glfwGetTime());
		delta_time = time_start - last_time_start; 
		last_time_start = time_start;

		set_time_frame(time_frames, delta_time, fbuff_size);
		glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_handle.update_state();
		c_handle.update_state();
		c_handle.update_vec(delta_time);
		if (m_handle.lm_pressed) std::cout << "mouse pos: " << m_handle.n_xpos << " " << m_handle.n_ypos << "\n";
		if (m_handle.lm_pressed) std::cout << "mouse fixed pos: " << m_handle.get_coords_str() << "\n";
		input_handle(ih_vars);


		glm::mat4 model{1.0f};
		m_projection = glm::ortho(0.0f, static_cast<float>(scr_w), 0.0f, static_cast<float>(scr_h), -1.0f, 3.0f);

		glUseProgram(flat_shader.gl_id);
		
		rinfo_fade.scale[0] = 300.0f * v3_dumm[0];
		rinfo.clear();

		leditor.process(grid_color, g_scr_size);

		npc_player.draw(m_projection);
		npc_slime.draw(m_projection);
		npc_slime2.draw(m_projection);
		npc_chest.draw(m_projection);
		npc_boost.draw(m_projection);
		npc_portal.draw(m_projection);
		npc_bat.draw(m_projection);
		text_cntrl.text = std::to_string(p_player.score);
		text_cntrl.draw(&rinfo);
		display_dead(glm::vec3(start_scr_w/2.0f, start_scr_h/2.0f-23.0f, 1.0f), &npc_player, skeleton_texture, &rinfo, &texture_shader, VAO);
		
		render_basic_texture(VAO, coin_position, glm::vec3(50.0f, 50.0f, 1.0f), texture_shader, coin_texture, &rinfo);
		render_handle(rinfo, m_projection, g_scr_size);

		int curr_fps{get_fps(time_frames, fbuff_size)};
		if (curr_fps < lowest_fps) 
		{
			lowest_fps = curr_fps;
			//std::cout << "New lowest FPS: " << curr_fps << "\n"; //Debug
		}// else if (curr_fps < notify_fps_drop) printf("FPS dropped lower %i: %i\n", notify_fps_drop, curr_fps);

		float curr_perf{static_cast<float>(glfwGetTime() - time_start)};
		if (!performance) performance = curr_perf;
		else performance = (performance + curr_perf)/2.0f;
		if (curr_perf > worst_perf) worst_perf = curr_perf;
		if (curr_perf< best_perf) best_perf = curr_perf;
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/*
	std::cout << "\n//////////////\nLowest recorded frame rate: " << lowest_fps << "\n";
	std::cout << "Average performance (lower better): " << performance << "\n";
	std::string f_perf_info{"Worst frame perfomance: " + std::to_string(worst_perf) + "; best frame perfomance: " + std::to_string(best_perf) + "\n"};
	std::cout <<  f_perf_info;
	system("date >> ./performance_records.txt");
	file::append("performance_records.txt", "Average perfomance: " + std::to_string(performance) + "\n" + f_perf_info + "Total work time: " + std::to_string(glfwGetTime()) + "\n\n");
	std::cout << "Work time: " << glfwGetTime() << "\n";
	*/
	glfwTerminate();
	return 0;
}
