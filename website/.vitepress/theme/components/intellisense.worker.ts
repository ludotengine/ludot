/// <reference lib="webworker" />
let runtime: Promise<any> | undefined
let engineApi: Promise<string> | undefined
self.onmessage = async ({ data }) => {
	try {
		runtime ||= import(/* @vite-ignore */ data.runtime).then((module) =>
			module.default({ print() {}, printErr() {} }),
		)
		if (data.engine)
			engineApi ||= fetch(data.api).then((response) => {
				if (!response.ok)
					throw new Error('Engine definitions could not load.')
				return response.text()
			})
		const [module, api] = await Promise.all([
			runtime,
			data.engine ? engineApi : '',
		])
		const result = module.ccall(
			'queryScript',
			'string',
			['string', 'number', 'number', 'string'],
			[data.code, data.line, data.column, api || ''],
		)
		self.postMessage({ id: data.id, ...JSON.parse(result) })
	} catch (error) {
		self.postMessage({
			id: data.id,
			error: error instanceof Error ? error.message : String(error),
		})
	}
}
