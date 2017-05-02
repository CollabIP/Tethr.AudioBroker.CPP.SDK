#pragma once

namespace tethr {
	enum CallStatus
	{
		/// <summary>
		/// The call currently in progress.
		/// </summary>
		InProgress,

		/// <summary>
		/// The Call has completed, but we are still processing it.
		/// </summary>
		Concluded,

		/// <summary>
		/// The call has completed, and we are done processing it.
		/// </summary>
		Complete,

		/// <summary>
		/// No call was found for the session.
		/// </summary>
		NotFound,

		Error
	};
}
