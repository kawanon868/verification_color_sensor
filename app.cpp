#include "ev3api.h"
#include "app.h"

#include <cmath>

#include "./include/Clock.h"
#include "./include/ColorSensor.h"

void main_task(intptr_t unused) {

	const int N_ITE = 1000; // 検証用の繰り返し回数.
	const int SLEEP_DURATION = 4; // 検証時のスリープ時間. ETロボコンおなじみの4[ms].

	/* new を使用したインスタンス生成 */
	ev3api::Clock *clock = new ev3api::Clock();
	ev3api::ColorSensor *colorSensor = new ev3api::ColorSensor(PORT_2);

	/* 初期化処理 */
	clock->reset();

	/* 検証1 : ev3api::ColorSensor.getBrightness() の検証 */
	{
		colorSensor->getBrightness(); // ウォーミングアップのために一度反射光を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < N_ITE; i++) {
			colorSensor->getBrightness(); // 反射光を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_1 : %5d [ms]\n", (t2 - t1));
	}

	/* 検証2 : ev3api::ColorSensor.getRawColor() の検証 */
	{
		rgb_raw_t trash_rgb;
		colorSensor->getRawColor(trash_rgb); // ウォーミングアップのために一度RGB Raw値を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < N_ITE; i++) {
			colorSensor->getRawColor(trash_rgb); // RGB Raw値を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_2 : %5d [ms]\n", (t2 - t1));
	}

	/* 検証3 : 上二つの関数を交互に使用する時の検証その1 */
	{
		rgb_raw_t trash_rgb;
		colorSensor->getBrightness(); // ウォーミングアップのために一度反射光を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < (int)(N_ITE / 2); i++) {
			colorSensor->getBrightness(); // 反射光を測定.
			clock->sleep(SLEEP_DURATION);
			colorSensor->getRawColor(trash_rgb); // RGB Raw値を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_3 : %5d [ms]\n", (t2 - t1));
	}

	/* 検証4 : 上二つの関数を交互に使用する時の検証その2 */
	{
		rgb_raw_t trash_rgb;

		colorSensor->getRawColor(trash_rgb); // ウォーミングアップのために一度RGB Raw値を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t_bright_to_rgb[N_ITE]; // 反射光モードからRGBモードに切り替わるまでの時間 [ms].
		int t_rgb_to_bright[N_ITE]; // RGBモードから反射光モードに切り替わるまでの時間 [ms].
		int t1, t2, t3; // 各タイミングにおける時間 [ms].

		for (int i = 0; i < N_ITE; i++) {
			t1 = (int)clock->now();
			colorSensor->getBrightness();
			t2 = (int)clock->now();
			colorSensor->getRawColor(trash_rgb);
			t3 = (int)clock->now();

			t_bright_to_rgb[i] = t3 - t2;
			t_rgb_to_bright[i] = t2 - t1;
		}

		// 平均と標準偏差を求めるための計算.
		int t_sum_bright_to_rgb = 0; // 反射光モードからRGBモードに切り替わるまでの時間の合計 [ms].
		int t_sum_rgb_to_bright = 0; // RGBモードから反射光モードに切り替わるまでの時間の合計 [ms].
		int t_square_sum_bright_to_rgb = 0; // 時間の二乗の合計 [ms]. 標準偏差の計算に必要.
		int t_square_sum_rgb_to_bright = 0; // 時間の二乗の合計 [ms]. 標準偏差の計算に必要.

		for (int i = 0; i < N_ITE; i++) {
			t_sum_bright_to_rgb += t_bright_to_rgb[i];
			t_sum_rgb_to_bright += t_rgb_to_bright[i];
			t_square_sum_bright_to_rgb += t_bright_to_rgb[i] * t_bright_to_rgb[i];
			t_square_sum_rgb_to_bright += t_rgb_to_bright[i] * t_rgb_to_bright[i];
		}

		// 平均を求める.
		float t_mean_bright_to_rgb = (float)t_sum_bright_to_rgb / (float)N_ITE;
		float t_mean_rgb_to_bright = (float)t_sum_rgb_to_bright / (float)N_ITE;

		// 2乗平均を求める.
		float t_square_mean_bright_to_rgb = (float)t_square_sum_bright_to_rgb / (float)N_ITE;
		float t_square_mean_rgb_to_bright = (float)t_square_sum_rgb_to_bright / (float)N_ITE;

		// 標準偏差を求める.
		float t_sd_bright_to_rgb = std::sqrt(t_square_mean_bright_to_rgb - std::pow(t_mean_bright_to_rgb, 2.0f));
		float t_sd_rgb_to_bright = std::sqrt(t_square_mean_rgb_to_bright - std::pow(t_mean_rgb_to_bright, 2.0f));

		// 結果をタスクログに表示.
		printf("Test_4 (bright to RGB) : \n");
		printf("Ave : %5.2f\nSD  : %5.2f\n", t_mean_bright_to_rgb, t_sd_bright_to_rgb);
		printf("Test_4 (RGB to bright) : \n");
		printf("Ave : %5.2f\nSD  : %5.2f\n", t_mean_rgb_to_bright, t_sd_rgb_to_bright);

	}

	/* タスク終了処理 */
	delete(colorSensor);
	delete(clock);
	ext_tsk();
}
