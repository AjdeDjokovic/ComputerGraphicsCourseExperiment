#include <igl/readOFF.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>
#include <iostream>
#include "tutorial_shared_path.h"
#include <igl/unproject_onto_mesh.h>

int main(int argc, char* argv[])
{
	Eigen::MatrixXd V, C;
	Eigen::MatrixXi F;

	// Load a mesh in OFF format
	igl::readOFF(TUTORIAL_SHARED_PATH "/bunny.off", V, F);

	// Init the viewer
	igl::opengl::glfw::Viewer viewer;
	C = Eigen::MatrixXd::Constant(F.rows(), 3, 1);
	viewer.callback_mouse_down =
		[&V, &F, &C](igl::opengl::glfw::Viewer& viewer, int, int)->bool
	{
		int fid;
		Eigen::Vector3f bc;
		// Cast a ray in the view direction starting from the mouse position
		double x = viewer.current_mouse_x;
		double y = viewer.core().viewport(3) - viewer.current_mouse_y;
		if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view,
			viewer.core().proj, viewer.core().viewport, V, F, fid, bc))
		{
			// paint hit red
			C.row(fid) << 1, 0, 0;
			viewer.data().set_colors(C);
			return true;
		}
		return false;
	};
	viewer.callback_mouse_up =
		[&V, &F, &C](igl::opengl::glfw::Viewer& viewer, int, int)->bool
	{
		int fid;
		Eigen::Vector3f bc;
		// Cast a ray in the view direction starting from the mouse position
		double x = viewer.current_mouse_x;
		double y = viewer.core().viewport(3) - viewer.current_mouse_y;
		if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view,
			viewer.core().proj, viewer.core().viewport, V, F, fid, bc))
		{
			// paint hit red
			C.row(fid) << 1, 1, 1;
			viewer.data().set_colors(C);
			return true;
		}
		return false;
	};

	// Attach a menu plugin
	igl::opengl::glfw::imgui::ImGuiMenu menu;
	viewer.plugins.push_back(&menu);

	// Customize the menu
	double doubleVariable = 0.1f; // Shared between two menus

	// Add content to the default menu window
	menu.callback_draw_viewer_menu = [&]()
	{
		// Draw parent menu content
		menu.draw_viewer_menu();

		// Add new group
		if (ImGui::CollapsingHeader("New Group", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Expose variable directly ...
			ImGui::InputDouble("double", &doubleVariable, 0, 0, "%.4f");

			// ... or using a custom callback
			static bool boolVariable = true;
			if (ImGui::Checkbox("bool", &boolVariable))
			{
				// do something
				std::cout << "boolVariable: " << std::boolalpha << boolVariable << std::endl;
			}

			// Expose an enumeration type
			enum Orientation { Up = 0, Down, Left, Right };
			static Orientation dir = Up;
			ImGui::Combo("Direction", (int*)(&dir), "Up\0Down\0Left\0Right\0\0");

			// We can also use a std::vector<std::string> defined dynamically
			static int num_choices = 3;
			static std::vector<std::string> choices;
			static int idx_choice = 0;
			if (ImGui::InputInt("Num letters", &num_choices))
			{
				num_choices = std::max(1, std::min(26, num_choices));
			}
			if (num_choices != (int)choices.size())
			{
				choices.resize(num_choices);
				for (int i = 0; i < num_choices; ++i)
					choices[i] = std::string(1, 'A' + i);
				if (idx_choice >= num_choices)
					idx_choice = num_choices - 1;
			}
			ImGui::Combo("Letter", &idx_choice, choices);

			// Add a button
			if (ImGui::Button("Print Hello", ImVec2(-1, 0)))
			{
				std::cout << "Hello\n";
			}
		}
	};

	// Draw additional windows
	menu.callback_draw_custom_window = [&]()
	{
		// Define next window position + size
		ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling(), 10), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 160), ImGuiSetCond_FirstUseEver);
		ImGui::Begin(
			"New Window", nullptr,
			ImGuiWindowFlags_NoSavedSettings
		);

		// Expose the same variable directly ...
		ImGui::PushItemWidth(-80);
		ImGui::DragScalar("double", ImGuiDataType_Double, &doubleVariable, 0.1, 0, 0, "%.4f");
		ImGui::PopItemWidth();

		static std::string str = "bunny";
		ImGui::InputText("Name", str);

		ImGui::End();
	};

	// Plot the mesh
	viewer.data().set_mesh(V, F);
	viewer.data().set_colors(C);
	viewer.launch();
}
