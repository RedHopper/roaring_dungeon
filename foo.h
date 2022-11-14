/*
 This header file contatins other essential functions that are being used
 to compile main file.
 */

struct slime_params
{
	char moving_dir{};
	void* player_handle{};
};

struct player_params
{
	int score{};
	int scr_death_bounds{1};
	int scr_death_kill{2};
	int scr_bat_kill{4};
	int scr_chest_pick{10};
};

bool charin(const char a, const char* str)
{
	int count{};
	int i{};
	while (true)
	{
		if (!str[i]) break;
		if (str[i] == a) return true;
		++i;
	}
	return false;
}

bool charin_vec(const char a, const std::vector<char>& vec)
{
	for (int i; i < vec.size(); ++i)
	{
		if (vec[i] == a) return true;
	}
	return false;
}

bool int_in_vec(int in, std::vector<int>& vec)
{
	for (int i{}; i < vec.size(); ++i) if (vec[i] == in) return true;
	return false;
}

int index_int(int in, std::vector<int>& vec)
{
	for (int i{}; i < vec.size(); ++i) if (vec[i] == in) return i;
	return -1;
}

template <typename T>
int index_vec(const T& obj, const std::vector<T>& vec)
{
	for (int i{}; i < vec.size(); ++i)
	{
		if (vec.at(i) == obj) return i;
	}
	return -1;
}

void split_str(std::string str, std::vector<std::string>* vec, char splitting_char = ',', char skip_char = ' ')
{
	std::string cache;
	for (int i{}; i < str.size(); ++i)
	{
		if (str[i] == splitting_char)
		{
			if (cache.size() > 0) vec->push_back(cache);
			cache = "";
			continue;
		} else if (str[i] == skip_char) continue;
		cache += str[i];
	}
	if (cache.size() > 0 && cache[0] != splitting_char && cache[0] != skip_char) vec->push_back(cache);
}


template <typename T>
T choose_rand(const std::vector<T>& vec, int rand_int)
{
	if (vec.size() == 1) return vec[0];
	else if (vec.size() > 1) return vec[rand_int%vec.size()];
	T zero{};
	return zero;
}

namespace file
{
	std::string read(std::string path)
	{
		std::fstream file;
		file.open(path, std::ios::in);
		if (!file.is_open())
	       	{
			std::cerr << "[*]Error: file::read() - can't open file with path: " << path << "\n";
			return "";
		}
		std::string cache;
		std::string result;
		while(getline(file, cache))
		{
			result += cache + "\n";
		}
		file.close();
		return result;
	}
	void append(std::string path, std::string to_append)
	{
		std::fstream file;
		std::string file_read{read(path)};
		file.open(path, std::ios::out);
		if (!file.is_open())
		{
			std::cerr << "[*]Error: file::append() - can't open file with path: " << path << "\n";
			return;
		}
		file << (file_read + to_append); 
		file.close();
	}
};

std::string chvec_to_str(const std::vector<char>& vec)
{
	if (!vec.size()) return "vec_empty";
	std::string result;
	for (int i{}; i < vec.size(); ++i)
	{
		result += std::to_string(vec.at(i)) + " ";
	}
	if (result.size() > 0) result.pop_back();
	return result;
}


std::string vec_to_str(glm::vec3 to_str, int dimensions = 3)
{
	std::string result;
	result += std::to_string(to_str[0]) + " ";
	if (dimensions >= 2)result += std::to_string(to_str[1]) + " ";
	if (dimensions >= 3) result += std::to_string(to_str[2]);
	if (dimensions < 3) result.pop_back();
	return result;
}
std::string vec_to_str(glm::vec4 to_str)
{
	std::string result;
	result += std::to_string(to_str[0]) + " ";
	result += std::to_string(to_str[1]) + " ";
	result += std::to_string(to_str[2]) + " ";
	result += std::to_string(to_str[3]);
	return result;
}
glm::vec4 vec34(glm::vec3 vec, float w)
{
	return glm::vec4(vec[0], vec[1], vec[2], w);
}

bool vec3_equal(const glm::vec3& vec1, const glm::vec3& vec2, float error = 0.00001f, bool ignore_z = true)
{
	if (abs(vec1[0]-vec2[0]) > error) return false;
	if (abs(vec1[1]-vec2[1]) > error) return false;
	if (!ignore_z && abs(vec1[2] - vec2[2]) > error) return false;
	return true;
}

bool float_equal(const float f1, const float f2)
{
	if (abs(f1-f2) > 0.00001f)
	{
		return false;
	}
	return true;
}
bool vec_moved(glm::vec3& curr_pos, const glm::vec3& target_pos, glm::vec3& moving_vec)
{
	bool moved_x{false};
	bool moved_y{false};
	if (float_equal(curr_pos[0], target_pos[0])) moved_x = true;
	if (float_equal(curr_pos[1], target_pos[1])) moved_y = true;
	if (moving_vec[0] > 0 && !float_equal(moving_vec[0], 0.0f))
	{
		if (curr_pos[0] > target_pos[0])
		{
			moved_x = true;
		}
	} else if (moving_vec[0] < 0 && !float_equal(moving_vec[0], 0.0f))
	{
		if (curr_pos[0] < target_pos[0])
		{
			moved_x = true;
		}
	}
	if (moving_vec[1] > 0 && !float_equal(moving_vec[1], 0.0f))
	{
		if (curr_pos[1] > target_pos[1])
		{
			moved_y = true;
		}
	} else if (moving_vec[1] < 0 && !float_equal(moving_vec[1], 0.0f))
	{
		if (curr_pos[1] < target_pos[1])
		{
			moved_y = true;
		}
	}
	if (moved_x) moving_vec[0] = 0.0f;
	if (moved_y) moving_vec[1] = 0.0f;
	if (float_equal(moving_vec[0], 0.0f)) curr_pos[0] = target_pos[0];
	if (float_equal(moving_vec[1], 0.0f)) curr_pos[1] = target_pos[1];
	if (moved_x && moved_y) return true;
	else if (vec3_equal(curr_pos, target_pos ,0.1f)) return true;
	return false;
}

template <typename T>
bool obj_in_arr(T obj, const T* arr, int arr_size)
{
	bool obj_is_in{};
	for (int i{}; i < arr_size; ++i)
	{
		if (arr[i] == obj) return true;
	}
	return obj_is_in;
}

namespace td{ //two dimensional operations
	glm::vec3 vec_abs(const glm::vec3& vec)
	{
		return glm::vec3(abs(vec[0]), abs(vec[1]), abs(vec[2]));
	}
	glm::vec3 center_coord(const glm::vec3& coord, const glm::vec3& curr_center, const glm::vec3& init_center, const float mod)
	{
		glm::vec3 curr_pos{coord};
		const glm::vec3 rel_pos{(curr_pos - init_center) * mod};
		return (rel_pos + curr_center);
	}
	void get_neigh_poses(const glm::vec3& pos, const float moving_step, std::vector<glm::vec3>* vec)
	{
		if (!vec)
		{
			std::cerr << "[E] td::get_neigh_poses() - vec pointer is zero\n";
			return;
		}
		vec->clear();
		vec->push_back(glm::vec3(pos[0]+moving_step, pos[1], pos[2]));
		vec->push_back(glm::vec3(pos[0], pos[1]+moving_step, pos[2]));
		vec->push_back(glm::vec3(pos[0]-moving_step, pos[1], pos[2]));
		vec->push_back(glm::vec3(pos[0], pos[1]-moving_step, pos[2]));
		return;
	}
	char turn_dir(char dir, bool clockwise = true)
	{
		const char cw[4]{'u', 'r', 'd', 'l'};
		auto found{std::find(cw, &cw[4], dir)};
		if (found != &cw[4])
		{
			if(clockwise)
			{
				found += 1;
				if (found == &cw[4])
				{
					return cw[0];
				}
				return *found;
			}
			else if (!clockwise)
			{
				found -= 1;
				if (found == &cw[-1])
				{
					return cw[3];
				}
				return *found;
			}
		}
		return 0;
	}
	char mvec_to_dir(const glm::vec3& moving_vec)
	{
		if (abs(moving_vec[0]) > abs(moving_vec[1]))
		{
			if (moving_vec[0] > 0.001f) return 'r';
			else return 'l';
		}
		else
		{
			if (moving_vec[1] > 0.001f) return 'u';
			else return 'd';
		}
		return 'u';
	}
	char get_key_dir(GLFWwindow* window)
	{
		if (!window) return 0;
		if (glfwGetKey(window, GLFW_KEY_UP)) return 'u';
		if (glfwGetKey(window, GLFW_KEY_DOWN)) return 'd';
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) return 'r';
		if (glfwGetKey(window, GLFW_KEY_LEFT)) return 'l';
		return 0;
	}
	float dir_tor(const char dir) // direction to rotation in degrees
	{
		if (dir == 'u') return 0.0f;
		if (dir == 'd') return 180.0f;
		if (dir == 'r') return 90.0f;
		if (dir == 'l') return 270.0f;
		return 0.0f;
	}
	char rand_dir(int rand_int)
	{
		rand_int = (rand_int%4);
		if (rand_int == 0) return 'u';
		if (rand_int == 1) return 'd';
		if (rand_int == 2) return 'r';
		if (rand_int == 3) return 'l';
		std::cerr << "[*] Error: td::rand_dir() - this must not be possible. rand_int is: " << rand_int << "\n";
		return 'r';
	}
	char opposite_dir(char dir)
	{
		if (dir == 'u') return 'd';
		if (dir == 'd') return 'u';
		if (dir == 'r') return 'l';
		if (dir == 'l') return 'r';
		std::cerr << "[*] Warning: td::opposite_dir() - Bad direction: " << dir << "\n";
		return 'r';

	}
	glm::vec3 dir_to_vec(const char dir, const float step)
	{
		glm::vec3 vec{0.0f};
		if (dir == 'u') vec[1] += step; 
		else if (dir == 'd') vec[1] -= step; 
		else if (dir == 'r') vec[0] += step; 
		else if (dir == 'l') vec[0] -= step; 
		if (vec[0] == 0.0f && vec[1] == 0.0f) std::cerr << "[*]Warning: td::dir_to_vec() - bad dir: " << dir << "\n";
		return vec;
	}
	glm::vec3 fix_rotation(glm::vec3 lpos, float rotation)
	{
		if (float_equal(rotation, 0.0f)) return lpos;
		else if (float_equal(rotation, 90.0f)) return glm::vec3(lpos[1], -lpos[0], lpos[2]);
		else if (float_equal(rotation, 180.0f)) return glm::vec3(-lpos[0], -lpos[1], lpos[2]);
		else if (float_equal(rotation, 270.0f)) return glm::vec3(-lpos[1], lpos[0], lpos[2]);
		std::cerr << "[*]Error: td::fix_rotation() - Can't decide what to do with rotation " << rotation << "\n";
		return lpos;
	}
}

struct lbutton_info //last button info
{
	char direction{};
	float set_time{};
	float allowed_diff{};
};

glm::vec2 mpos_to_coords(float x, float y, float w, float h)
{
	return glm::vec2(x*w, y*h);
}
std::string vec2_str(glm::vec2 vec)
{
	return std::to_string(vec[0]) + " " + std::to_string(vec[1]);
}
