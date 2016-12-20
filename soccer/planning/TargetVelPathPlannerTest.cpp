#include <gtest/gtest.h>
#include <Geometry2d/Point.hpp>
#include "TargetVelPathPlanner.hpp"
#include "planning/MotionCommand.hpp"

using namespace Geometry2d;

namespace Planning {

TEST(TargetVelPathPlannerTest, run) {
    MotionInstant startInstant({0, 0}, {0, 0});
    WorldVelTargetCommand cmd(Point(0, 1));

    MotionConstraints motionConstraints;
    ShapeSet obstacles;
    obstacles.add(std::make_shared<Rect>(Point(-1, 5), Point(1, 4)));

    SystemState systemState;

    TargetVelPathPlanner planner;
    std::vector<DynamicObstacle> dynamicObstacles;
    SinglePlanRequest request(startInstant, cmd, RobotConstraints(), obstacles,
                              dynamicObstacles, systemState,
                              nullptr,  // previous path
                              0         // shellID
                              );
    auto path = planner.run(request);

    ASSERT_NE(nullptr, path) << "Planner returned null path";

    // Ensure that the path is obstacle-free
    RJ::Seconds hitTime;
    EXPECT_FALSE(path->hit(obstacles, 0s, &hitTime))
        << "Returned path hits obstacles";

    // Ensure that the path moves in the direction of the target world velocity
    // (positive y-axis)
    boost::optional<RobotInstant> instant = path->evaluate(100ms);
    ASSERT_TRUE(instant);
    EXPECT_FLOAT_EQ(0, instant->motion.pos.x());
    EXPECT_GT(instant->motion.pos.y(), 0);
}

}  // namespace Planning
